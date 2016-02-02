

void initail_LED(){
  // กำหนดค่าเริ่มต้น MAX7219 device
lc.shutdown(0,false); // Enable display
lc.setIntensity(0,1); // ปรับระดับความสว่างได้ 16 ระดับ 0 คือค่าต่ำสุด 15 คือค่าสูงสุด
lc.clearDisplay(0); // Clear display register
}




void LED_SAT(){
  lc.setRow(0,7,B01011011); // S
  lc.setDigit(0,6,10,false); // A
  lc.setRow(0,5,B10001111); //t.
  lc.setDigit(0,4,sat,false); 

  lc.setRow(0,1,B11000111); //F.
  lc.setDigit(0,0,sat_fix,false); 
}

