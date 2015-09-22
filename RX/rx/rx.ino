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

unsigned long count = 0;
uint8_t buf[32];
uint8_t len = sizeof(buf);


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

  float lad1;
  float log1;
  float lad2 = 18.783161;
  float log2 = 98.978784;
 
  int Start_Altiture = 0;
  int Altiture;
  int min_Altiture = 1;           //ปรับค่าได้
  int max_Altiture = 200;         //ปรับค่าได้

  float Cal_distance;
  float Cal_compass; //มุมเป้าหมาย
 
  float Angle_Qand_in_moment;  //มุมของตัวเอง
        

  int caseSwitch = 1;

  int CH_GPS_Fail =0; // ไว้หน่วงเวลา



  
  
  

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

  Read_GPS();                                 //อ่านค่า Altiture ครั้งแรก เพื่อเก็บค่า Alt ที่พื้น
  Start_Altiture = NazaDecoder.getGpsAlt();
}






  
  
void loop()
{
   Read_GPS();
}




//ทำ ระบบ คาริเบต altiture ของ mix กะ max
         // min_Altiture = Altiture - min_Altiture ;
          //max_Altiture = Altiture + max_Altiture ;
