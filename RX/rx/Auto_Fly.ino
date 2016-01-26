



void Auto_Fly(){   
      
     myservo_AUX2.write(128);      // adjust swith AUX >>>127(GPS mode)
     
     if(!check_gps_home){ //เก็บค่าพิกัด Home
      home_lat = lad1;
      home_long = log1;
      check_gps_home = true;
     }
     
     
     /*
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
       */
       ////////////////////////////////////////// auto fly _ New Logic ///////////////////////////////////////////////////

    if(check_go_to_gps){ //ขาไป
      Cal_compass = calc_bearing(lad1 ,log1 ,lad2 ,log2); //มุมเป้าหมายที่ต้องการไป
      Cal_distance = calc_dist(lad1 ,log1 ,lad2 ,log2); //ระยะทางจากเครื่องไปถึงเป้าหมาย
      //Angle_Qand_in_moment //มุมของเครื่อง ณ ตอนนี้ //รับค่าที่ Read_GPS
    }else{ //ขากลับไป Home
      Cal_compass = calc_bearing(lad1 ,log1 ,home_lat ,home_long); 
      Cal_distance = calc_dist(lad1 ,log1 ,home_lat ,home_long); 
      }

    switch (caseSwitch)
    {
    case 1:  //บินขึ้น
      //if (Altiture >= max_Altiture) { //ถ้าความสูง >= ระยะที่กำหนด
      
      if (High >= min_High && High <= max_High) {
        caseSwitch=2;
      } else if (High <= min_High) {
        takeOff();
        delay(100);
      }
      Serial.print("caseSwitch_now : 1"); 
      //Serial.print("\t Altiture:"); Serial.print(Altiture);
      //Serial.print("\t max_Altiture:"); Serial.print(max_Altiture);
      //Serial.print("\t Nasa Al: "); Serial.print(NazaDecoder.getGpsAlt()); 
      Serial.print("\t Nasa Lat: "); Serial.print(lad1,10);
      Serial.print("\t Nasa Lon: "); Serial.print(log1,10); 
      Serial.print("\t Nasa Sat: "); Serial.print(sat_fix); 
      Serial.print("\t High: "); Serial.print(High);
       
      Serial.println();
      break;
    case 2: //หมุนไปหาตำแหน่ง
      Rolling_follow_GPS();
      if (abs (Angle_Qand_in_moment - Cal_compass) <= 1){
        caseSwitch = 3;
      }
      check_High();
      Serial.print("caseSwitch_now : 2");
      Serial.print("\t Angle_Qand_in_moment:"); Serial.print(Angle_Qand_in_moment);
      Serial.print("\t Cal_compass:"); Serial.print(Cal_compass);
      Serial.print("\t A-C:"); Serial.print(abs (Angle_Qand_in_moment - Cal_compass));
      Serial.println();
      break;
    case 3: //แล่นไปหาตำแหน่ง + หมุนไปหาตำแหน่งด้วย
      Run_follow_GPS();
      delay(500);
      Rolling_follow_GPS ();
      delay(500);
     // cal_baering.Chang_GPS = 0;
      if(Cal_distance <= 1.5){    //<<<<<< ตั้งระยะเผื่อตรงนี้นะ
        caseSwitch = 4;
      } 
      check_High();
      Serial.print("caseSwitch_now : 3");
      Serial.print("\t Cal_distance:"); Serial.print(Cal_distance);
      Serial.print("\t Cal_compass:"); Serial.print(Cal_compass);
      Serial.print("\t A-C:"); Serial.print(abs (Angle_Qand_in_moment - Cal_compass));
      Serial.println();
      break;
    case 4: //ลงจอด
      landing();
      if (High <= 12) { //ถ้าความสูง >= ระยะที่กำหนด
        caseSwitch=5;
        
      }
      Serial.print("caseSwitch_now : 4");
      //Serial.print("\t Altiture:"); Serial.print(Altiture);
      //Serial.print("\t min_Altiture:"); Serial.print(min_Altiture);
      Serial.print("\t High:"); Serial.print(High);
      Serial.println();
      break;


     case 5:
        Serial.print("caseSwitch_now : 5");
        if(check_go_to_gps){
         check_go_to_gps = false;
          caseSwitch=1;
        }else{
          caseSwitch = 6;
        }
      break;

      
    case 6:
                Serial.print("caseSwitch_now : 6");
                myservo_yaw.write(143);   //ดับเครื่อง 
                myservo_up.write(45);
                myservo_roll.write(45);
                 myservo_pitch.write(45);
                Serial.println();
      //if(cal_baering.Chang_GPS == 1){
      //  caseSwitch = 1;
      //  cal_baering.Chang_GPS = 0;
     // }
      break;
    default:
     
        
      break;
    } 

    
    

    //Serial.print("\tcaseSwitch : ");    Serial.println(caseSwitch);  
       
     
}



void Rolling_follow_GPS(){
  
    float cut_Rolling = 1;  //มุมเผื่อ
     
    if (abs(Angle_Qand_in_moment - Cal_compass) >= cut_Rolling){
         if (Angle_Qand_in_moment < 180) {
           if (Angle_Qand_in_moment + 180 > Cal_compass && Angle_Qand_in_moment < Cal_compass) { //หมุนขวา
                  myservo_yaw.write(105);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95);
           } else {                         //หมุนซ้าย
                  myservo_yaw.write(85);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95); 
           }
         } else if (Angle_Qand_in_moment > 180) {
           if (Angle_Qand_in_moment - 180 < Cal_compass && Angle_Qand_in_moment > Cal_compass) { //หมุนซ้าย
                  myservo_yaw.write(85);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95); 
           } else {                             //หมุนขวา
                  myservo_yaw.write(105);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95);
           }
         } else if (Angle_Qand_in_moment == 180) {
           if (Angle_Qand_in_moment > Cal_compass) {  //หมุนซ้าย
                  myservo_yaw.write(85);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95);
           } else {                             //หมุนขวา
                  myservo_yaw.write(105);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95);
           }
         } else if (Angle_Qand_in_moment == 0) {  //หมุนซ้าย
           if (Cal_compass > 180) {
                  myservo_yaw.write(85);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95); 
           } else {                                 //หมุนขวา
                  myservo_yaw.write(105);
                  myservo_up.write(86);
                  myservo_roll.write(95);
                  myservo_pitch.write(95);
           }
         } else {
           
         }
    }

  
}




void check_High(){
    myservo_up.write(93);
    if (High < min_High) {
        takeOff();
        delay(100);
      } else if (High > max_High) {
        landing();
        delay(100);
      }
  }




void Run_follow_GPS(){
      myservo_yaw.write(95);
      myservo_up.write(86);
      myservo_roll.write(95);
      myservo_pitch.write(110);
}

void takeOff(){//เครื่องขึ้น
    myservo_yaw.write(95);
    myservo_up.write(102);
    myservo_roll.write(95);
    myservo_pitch.write(95);
}

void  landing(){ //เครื่องลง
    myservo_yaw.write(95);
    myservo_up.write(83);
    myservo_roll.write(95);
    myservo_pitch.write(95);
    //if (Altiture <= min_Altiture) { //ถ้าความสูง <= 1 //เครื่องถึงพื้นแล้ว
      //ควรจะ delay หน่อย แต่ตอนนี้ยังไม่ได้ใส่
      //ใส่ปรับให้ caseSwitch 
    //}

    
  }


