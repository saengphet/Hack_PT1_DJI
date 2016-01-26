void Read_Sonar(){
    long duration, distance;
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
  //  delayMicroseconds(1000); - Removed this line
    delayMicroseconds(10); // Added this line
    //digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration) / 29.1;
    //Serial.print("distance: ");
    //Serial.println(distance);
    delay(50);
    High = distance;
    //return (distance);
  }
