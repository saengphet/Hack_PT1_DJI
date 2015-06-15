// nrf24_audio_rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create an audio digital receiver
// with the NRF24 class. 
// Works with the nrf24_audio_tx sample transmitter
// Connect audio output to pin 6, through a low pass filter consisting of a 1k resister and series followed by a 
// 0.0033 microfarad capacitor to ground (48kHz filter).
// The audio quality is poor: dont expect hi-fi!
// We have to change the PWM frequency to 62 kHz so we can get bandwidth reasonable 
// audio out through the low pass filter
// Tested on UNO



//
//            O       ^      O                   ^                                                      
//              \          /                     ^                                                      
//                \      /                       ^                                                      
//                  ----                       Pitch       < < <  Roll > > >          ^                         ^            
//                  |  |                         v                                      <                     >   
//                  ----                         v                                         <               >        
//                 /    \                        v                                             <       >       
//               /        \                                                                       Yaw
//             /            \                   
//           O                O                   



#include <NRF24.h>
#include <SPI.h>
#include <Servo.h> 
#include "NazaDecoderLib.h"




// Singleton instance of the radio
//NRF24 nrf24;
 NRF24 nrf24(9, 10); // CE/CSN
// NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pin





  int StartTime,Timer,CountTime,Time_nextState;
  
  int CH_StartTime = 0, timeToFailSaft = 0;
  
  int up_AIvalue = 45 , // value for write servo up
      up_AIvalue_MAX = 102,
      up_AIvalue_MIN = 45;
      
   int yaw_AIvalue = 90 , roll_AIvalue = 90 , pitch_AIvalue = 90 ;
      
  int CH_up_AIvalue = 0 , // Check for " up_AIvalue++ " take one time
      CH_up_AIvalue_TakeOff = 0, // Check for " up_AIvalue " take ++ or --
      CH_hool = 0, //Check for hool altiture one time
      CH_Start_TimeHool = 0, //Check for hool altiture one time
      CH_Takeoff = 0;
   
   //Fail_Safe   
  int Start_FailSafe_Time =0 , FailSafe_Time=0;
  int CH_FailSafe_Time = 0;
  
  
  

/**
 * Divides a given PWM pin frequency by a divisor.
 *
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 *
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 *
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 *
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
Servo myservo_yaw;
Servo myservo_up;
Servo myservo_roll;
Servo myservo_pitch;
Servo myservo_AUX1;
Servo myservo_AUX2;


void setup() 
{
  Serial.begin(115200);
  

  
  myservo_yaw.attach(4);
  myservo_up.attach(3);
  myservo_roll.attach(6);
  myservo_pitch.attach(5);
  myservo_AUX1.attach(7);
  myservo_AUX2.attach(8);
  

  
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("NRF24 init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setThisAddress((uint8_t*)"aurx1", 5))
    Serial.println("setThisAddress failed");

  if (!nrf24.setPayloadSize(32))
    Serial.println("setPayloadSize failed");
  if (!nrf24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm))
    Serial.println("setRF failed");    
  if (!nrf24.powerUpRx())
      Serial.println("powerOnRx failed");    
  // Here we change the PWM frequency so we can render audio with better quality
  setPwmFrequency(6, 1); // PWM output on pin 6 is 62 khz
  Serial.println("initialised");
}





unsigned long count = 0;
 uint8_t buf[32];
  uint8_t len = sizeof(buf);
  
  
void loop()
{
   //Timer = millis()/1000;
   GPS();
}
  
  int CH_GPS_Fail =0;
  void GPS()
{
  
    if(Serial.available())
  {
    uint8_t decodedMessage = NazaDecoder.decode(Serial.read());
    CH_GPS_Fail = 0;
     //Serial.print("decodedMessage:");
     //Serial.println(decodedMessage);
    switch (decodedMessage)
    {
      case NAZA_MESSAGE_GPS:
        //Serial.print("Lat: "); Serial.print(NazaDecoder.getLat(), 7);
//        Serial.print(", Lon: "); Serial.print(NazaDecoder.getLon(), 7);
//        Serial.print(", Alt: "); Serial.print(NazaDecoder.getGpsAlt(), 7);
//        Serial.print(", Fix: "); Serial.print(NazaDecoder.getFixType());
//        Serial.print(", Sat: "); Serial.println(NazaDecoder.getNumSat());
        break;
      case NAZA_MESSAGE_COMPASS:
        //Serial.print("Heading: "); Serial.println(NazaDecoder.getHeadingNc(), 2);
        RX_remote();
        break;
    }

  } else{
    if(CH_GPS_Fail>=20000){
         Serial.println("GPS Fail >>> Landing mode");
           myservo_yaw.write(95);
          myservo_up.write(83);
          myservo_roll.write(95);
          myservo_pitch.write(95);
    }
    CH_GPS_Fail++;
  }
  
}


  
  
  
  
  void RX_remote(){
     //nrf24.waitAvailable();
  if (nrf24.recv(buf, &len)) // 140 microsecs
  {
      CH_FailSafe_Time = 0 ; //if can recive signal >>> reset value in fail-safe mode
      // This delay was established experimentally to make sure the
      // buffer was exhausted just in time for the next packet to arrive     


    
     //vvvvvvvvvv Write Value to control  vvvvvvvvvvvvv
   
   
    if(buf[5]<50){                 // 1       Auto fly
      
     myservo_AUX2.write(127);
     
     if(CH_StartTime == 0){       // Save start time, When you come this mode and take one time.
       StartTime = millis()/100000;
     }   
     
     Timer = millis()/100000;  
     CountTime = Timer - StartTime;
     //Time_nextState = CountTime-Time_nextState;
     CH_StartTime=1;
      
     if(CH_StartTime == 1){   // Loop
       
          if(CountTime <= 2 && CH_Takeoff == 0){                                        // Start motor  and  hole in 2 sec
                yaw_AIvalue = 45;
                up_AIvalue = 45;
                roll_AIvalue = 45;
                pitch_AIvalue = 45;
          }
          
          if(CountTime >= 2  &&  CountTime <= 5   &&   CH_Takeoff == 0){                // Take off in 3 sec
                 up_AIvalue = up_AIvalue_MAX;
                 yaw_AIvalue = 95;
                 roll_AIvalue = 95;
                 pitch_AIvalue = 95;
                 if(CountTime == 4){                                                    // Take off is already.
                       CH_Takeoff=1;                      
                 }
          }
          
          if(CountTime >= 5  &&  CountTime <= 8){                                      // Hole in 3 sec
                 up_AIvalue = 86;
                 yaw_AIvalue = 95;
                 roll_AIvalue = 95;
                 pitch_AIvalue = 95;
          }
          
          if(CountTime >= 8  &&  CountTime <= 11){                                      // Landing in 3 sec
          
                 up_AIvalue = 83;
                 yaw_AIvalue = 95;
                 roll_AIvalue = 95;
                 pitch_AIvalue = 95;
          }
          
         if(CountTime >= 11){                                                            // Off
          
                yaw_AIvalue = 45;
                up_AIvalue = 45;
                roll_AIvalue = 45;
                pitch_AIvalue = 45;
          }
          
             
          up_AIvalue = constrain(up_AIvalue, up_AIvalue_MIN, up_AIvalue_MAX);
          
          myservo_yaw.write(yaw_AIvalue);
          myservo_up.write(up_AIvalue);
          myservo_roll.write(roll_AIvalue);
          myservo_pitch.write(pitch_AIvalue);
         
       
       
          Serial.print("StartTime : ");    Serial.print(StartTime);   Serial.print("\t CountTime: ");    Serial.print(CountTime);   Serial.print("\t up_AIvalue: ");    Serial.print(up_AIvalue);
          Serial.print("\t CH_Takeoff: ");    Serial.print(CH_Takeoff);  
         Serial.println("");
       
       
       
     }
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     /* // Old Logic

     
      if(CH_StartTime == 0){       
       StartTime = Timer;
     }
     
     CH_StartTime=1;
     CH_up_AIvalue = 0;
     
     if(CH_StartTime == 1){        //3
      
      
      if(Timer-StartTime <= 200){  // Start motor
        Serial.print("start");
        up_AIvalue = 45;
        myservo_yaw.write(up_AIvalue);
        myservo_up.write(up_AIvalue);
        myservo_roll.write(up_AIvalue);
        myservo_pitch.write(up_AIvalue);
      }
      
      if(Timer-StartTime >= 200){
        myservo_up.write(up_AIvalue);
        if(millis()/10 >=8 && CH_up_AIvalue == 0){  //  bulid dely myself   &&   take one time over this delay loop
          CH_up_AIvalue = 1;
          
          if(up_AIvalue <= up_AIvalue_MIN+5){
            CH_up_AIvalue_TakeOff = 0;
            CH_hool = 0;
          }if(up_AIvalue >= up_AIvalue_MAX-1){
            CH_up_AIvalue_TakeOff = 1;
          }
          
          if(CH_up_AIvalue_TakeOff == 0){    // MIN value and take something
            up_AIvalue++;
          }else if(CH_up_AIvalue_TakeOff == 1){   // MAX value and take something
            if(CH_hool == 0){        //hool altiture for 5 sec
              if(CH_Start_TimeHool==0){ Start_hool=Timer; CH_Start_TimeHool=1;}
              if(Timer - Start_hool <= 500){
                 up_AIvalue = up_AIvalue_MAX;
              }else { CH_hool=1; }
            }else{
              up_AIvalue--;
            }
          }
        }
          up_AIvalue = constrain(up_AIvalue, up_AIvalue_MIN, up_AIvalue_MAX);
          CH_up_AIvalue = 0;
       }         //4
     }          //3
      
      Serial.print("Timer: "); Serial.print(Timer);Serial.print(" \tStartTime: "); Serial.print(StartTime); Serial.print("\tup_AIvalue: "); Serial.print(up_AIvalue);
      Serial.print("\tCH_up_AIvalue_TakeOff: "); Serial.println(CH_up_AIvalue_TakeOff);
     
     */ // Old Logic
     
     
   } //   1      if(buf[5]<50)
   
   
   
   
      
   else {  // Manual fly
     Serial.print("Timer: "); Serial.println(Timer);       
     Serial.print((map(buf[0],0,180,45,143)));  Serial.print("\t");  
      Serial.print((map(buf[1],0,180,45,143)));  Serial.print("\t");  
      Serial.print((map(buf[2],0,180,45,143)));  Serial.print("\t");  
      Serial.print((map(buf[3],0,180,45,143)));  Serial.print("\t");  
      Serial.print(buf[4]);  Serial.print("\t"); 
      Serial.println(buf[5]);
      
      
      if(buf[0]==buf[0] && buf[1]==buf[1] && buf[2]==buf[2] && buf[3] == buf[3]) {
        timeToFailSaft++;
        
         Serial.print("timeToFailSaft : ");  Serial.println(timeToFailSaft); 
        
       // Serial.println(">>> Fail safe<<<");  
      }
      
     CH_StartTime = 0; 
     
     
      myservo_yaw.write(map(buf[0],0,180,45,143));
      myservo_up.write(map(buf[1],0,180,45,143));
      myservo_roll.write(map(buf[2],-5,180,45,143));
      myservo_pitch.write(map(buf[3],0,180,45,143));
      
      if(buf[4]<50){
        myservo_AUX1.write(45);
      }else   if(buf[4]>50 && buf[4]<200){
        myservo_AUX1.write(94);
      }else   if(buf[4]>200){
        myservo_AUX1.write(160);
      }
      
        //if(buf[5]<50){
       // myservo_AUX2.write(62);
     // }else   
      if(buf[5]>50 && buf[5]<200){
        myservo_AUX2.write(95);
      }else   if(buf[5]>200){
        myservo_AUX2.write(127);
      }
    } 

      
       //^^^^^^^ Write Value to control ^^^^^ 
      
    
    
   } else{
     Serial.println("Signal Lost >>> Go to Fail \"safe mode\"");
     Fail_Safe();
   }
  
  
}





void Fail_Safe(){

if(CH_FailSafe_Time == 0){
           Start_FailSafe_Time = millis()/100000;   
     }
     CH_FailSafe_Time = 1;
     FailSafe_Time = millis()/100000 - Start_FailSafe_Time; 
    
     if(CH_FailSafe_Time == 1){   // Loop
       
          if(FailSafe_Time >=  10){             // Start motor  and  hole in 2 sec
              Serial.print(" Go to Haome ");
              myservo_AUX2.write(77);
          }
          
     }
     
    Serial.print(" \tFailSafe_Time: ");    Serial.println(FailSafe_Time);



}
  
  
  




