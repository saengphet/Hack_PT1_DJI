//TX
#include <NRF24.h>
#include <SPI.h>

// Define pin            Max-Min Value
int yaw; //A0          _* 781-100
int up; //A1           _ 810-187
int roll; //A2         _* 945-246
int pitch; //A3        _* 795-214
int AUX1; //A6 left    _ 0-511-1023
int AUX2; //A7 right   _ 0-511-1023


 NRF24 nrf24(10, 9);

void setup() {
  Serial.begin(115200);
  setup_nrf ();

}

void loop() {
    read_value ();
    sent_value_nrf ();
    Serial.print("  yaw = ");
   Serial.print(yaw);Serial.print("\t");
   Serial.print(up);Serial.print("\t");
   Serial.print(roll);Serial.print("\t");
   Serial.print(pitch);Serial.print("\t");
   Serial.print(AUX1);Serial.print("\t");
   Serial.print(AUX2);
  delay(20);
}

void read_value () {
  // Read pin                   Tuning value to 0-180              Reverse value
  yaw = (analogRead(A0));    yaw = map(yaw,100,781,0,180);       yaw = 180-yaw;
  up = (analogRead(A1));     up = map(up,187,810,0,180);         
  roll = (analogRead(A2));   roll = map(roll,246,935,0,180);     roll = 180-roll;
  pitch = (analogRead(A3));  pitch = map(pitch,214,795,0,180);   pitch = 180-pitch;
  AUX1 = (analogRead(A6));   AUX1 = map(AUX1,0,1023,0,255);
  AUX2 = (analogRead(A7));   AUX2 = map(AUX2,0,1023,0,255);
}


void setup_nrf () {
 
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("NRF24 init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setPayloadSize(32))
    Serial.println("setPayloadSize failed");
  if (!nrf24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm))
    Serial.println("setRF failed");    
  // Enable the EN_DYN_ACK feature so we can use noack
  nrf24.spiWriteRegister(NRF24_REG_1D_FEATURE, NRF24_EN_DYN_ACK);
  //analogReference(INTERNAL); // So we can read 1Vp-p signals
  Serial.println("initialised");
}

void sent_value_nrf () {
  uint8_t buf[32];
  uint8_t i;
  
  
    buf[0] = yaw;
    buf[1] = up;
    buf[2] = roll;
    buf[3] = pitch;
    buf[4] = AUX1;
    buf[5] = AUX2;
    Serial.print("    send = ");
     Serial.print(buf[0]);  Serial.print("\t");
     Serial.print(buf[1]);  Serial.print("\t");
     Serial.print(buf[2]);  Serial.print("\t");
     Serial.print(buf[3]);  Serial.print("\t");
     Serial.print(buf[4]);  Serial.print("\t");
     Serial.println(buf[5]);  
 
   
  // Now send the samples NOACK (EN_DYN_ACK must be enabled first)
  // With 2Mbps, NOACK and 32 byte payload, can send about 1900 messages per sec
  if (!nrf24.setTransmitAddress((uint8_t*)"aurx1", 5))
   Serial.println("setTransmitAddress failed");
  // Send the data  
  if (!nrf24.send(buf, sizeof(buf), true)) // NOACK, 110 microsecs
     Serial.println("send failed");  
  // Transmission takes about 300 microsecs, of which about 130microsecs is transmitter startup time
  // and 160 microsecs is transmit time for 32 bytes+8 bytes overhead @ 2Mbps
  if (!nrf24.waitPacketSent())
     Serial.println("waitPacketSent failed");  
    digitalWrite(3, LOW);
}
