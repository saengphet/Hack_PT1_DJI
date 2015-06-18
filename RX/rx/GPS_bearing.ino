/*
    
   web for cal gps http://www.gpsvisualizer.com/calculators
Refereace
   dis   -    http://forum.arduino.cc/index.php?topic=27541.0
   angle   -   http://forum.arduino.cc/index.php?topic=102223.0

*/


/////
float calc_bearing(float flat1, float flon1, float flat2, float flon2)
{
  float calc;
  float bear_calc;

  float x = 69.1 * (flat2 - flat1); 
  float y = 69.1 * (flon2 - flon1) * cos(flat1/57.3);

  calc=atan2(y,x);

  bear_calc= degrees(calc);

  if(bear_calc<=1){
    bear_calc=360+bear_calc; 
  }
  return bear_calc;
}



////////

float calc_dist(float flat1, float flon1, float flat2, float flon2)
{
float dist_calc=0;
float dist_calc2=0;
float diflat=0;
float diflon=0;

//I've to spplit all the calculation in several steps. If i try to do it in a single line the arduino will explode.
diflat=radians(flat2-flat1);
flat1=radians(flat1);
flat2=radians(flat2);
diflon=radians((flon2)-(flon1));

dist_calc = (sin(diflat/2.0)*sin(diflat/2.0));
dist_calc2= cos(flat1);
dist_calc2*=cos(flat2);
dist_calc2*=sin(diflon/2.0);
dist_calc2*=sin(diflon/2.0);
dist_calc +=dist_calc2;

dist_calc=(2*atan2(sqrt(dist_calc),sqrt(1.0-dist_calc)));

dist_calc*=6371000.0; //Converting to meters
//Serial.println(dist_calc);
return dist_calc;
}
