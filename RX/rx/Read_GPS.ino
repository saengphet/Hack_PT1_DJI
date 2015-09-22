


void Read_GPS()
{
  
    if(Serial.available())
  {
    uint8_t decodedMessage = NazaDecoder.decode(Serial.read());
    
     //Serial.print("decodedMessage:");
     //Serial.println(decodedMessage);
    switch (decodedMessage)
    {
      case NAZA_MESSAGE_GPS:
//        Serial.print("Lat: ");   Serial.print(NazaDecoder.getLat(), 7);
//        Serial.print(", Lon: "); Serial.print(NazaDecoder.getLon(), 7);
//        Serial.print(", Alt: "); Serial.print(NazaDecoder.getGpsAlt(), 7);
//        Serial.print(", Fix: "); Serial.print(NazaDecoder.getFixType());
//        Serial.print(", Sat: "); Serial.println(NazaDecoder.getNumSat());
          lad1 = NazaDecoder.getLat();
          log1 = NazaDecoder.getLon();
          Altiture = abs(NazaDecoder.getGpsAlt() - Start_Altiture);

        break;
      case NAZA_MESSAGE_COMPASS:
        //Serial.print("Heading: "); Serial.println(NazaDecoder.getHeadingNc(), 2);
        Angle_Qand_in_moment = NazaDecoder.getHeadingNc(); //Get ค่ามุมของเครื่อง ณ ขณะนี้

        RX_remote();
        break;
    }
    CH_GPS_Fail = 0; //รีเซตเวลา GPS Time fail 

  } else{ //ถ้า GPS Fail ให้มัน ลงจอด
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
