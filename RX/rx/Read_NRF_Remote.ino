int RX_remote(){
  
     //nrf24.waitAvailable();
  if (nrf24.recv(buf, &len)) // 140 microsecs
  {
      CH_FailSafe_Time = 0 ; //if can recive signal >>> reset value in fail-safe mode
      // This delay was established experimentally to make sure the
      // buffer was exhausted just in time for the next packet to arrive     
      


     //vvvvvvvvvv Write Value to control  vvvvvvvvvvvvv
   
   
    if(buf[5]<50){                 //  Auto fly
        manual_fly = false;
        //Auto_Fly();
      
        
   }  
   else {  // Manual fly
      //Manual_Fly();
      manual_fly = true;
    } 
       //^^^^^^^ Write Value to control ^^^^^ 

  
    return  true;
   } 
   
   
   
   else{ //ถ้าสัญญาน Remote หาย
     return false;
     Serial.println("Signal Lost >>> Go to Fail \"safe mode\"");
     Landding();
   }
  
 
}







void Fail_Safe(){

if(CH_FailSafe_Time == 0){
           Start_FailSafe_Time = millis()/100000;   
     }
     CH_FailSafe_Time = 1;
     FailSafe_Time = millis()/100000 - Start_FailSafe_Time; 
    
     if(CH_FailSafe_Time == 1){   // Loop
       
          if(FailSafe_Time >=  10 ){             // Start motor  and  hole in 2 sec
              Serial.print(" Go to Haome ");
              myservo_AUX2.write(110);

          }
          
     }
     
    Serial.print(" \tFailSafe_Time: ");    Serial.println(FailSafe_Time);



}
