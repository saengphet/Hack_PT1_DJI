void Auto_Fly(){   
     myservo_AUX2.write(127);      // adjust swith AUX >>>127(GPS mode)
     
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
}

void Run_follow_GPS(){






  
}
