
void Manual_Fly(){
      Serial.print("Timer: "); Serial.println(Timer);       
      Serial.print((map(buf[0],0,180,45,143)));  Serial.print("\t");  
      Serial.print((map(buf[1],0,180,45,143)));  Serial.print("\t");  
      Serial.print((map(buf[2],0,180,45,143)));  Serial.print("\t");  
      Serial.print((map(buf[3],0,180,45,143)));  Serial.print("\t");  
      Serial.print(buf[4]);  Serial.print("\t"); 
      Serial.println(buf[5]);
      
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
