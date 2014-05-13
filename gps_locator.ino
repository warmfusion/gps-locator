#include <TinyGPS.h> //include TinyGPS library
#define LOW_POWER_MODE 0  //Low Power mode includes extra libraries making the compile a little bigger
#define DEBUG 0           //Serial logging at the price of more memory consumption?

#if LOW_POWER_MODE
#include <JeeLib.h> // Low power functions library
ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog
#endif

// Declare functions
void notLost();
boolean pollGPS();
float calc_dist(float, float, float, float);
// End of Function declaration

TinyGPS gps; //initialise GPS object
float targetLat=55.582913, targetLon=-1.835315;
float nearDist = 50, farDist = 500;
int nearPin = 5, farPin = 6, lostPin = 7;
int lostCount=0;

void setup()
{
  Serial.begin(9600);      //initialise serial port
  pinMode(nearPin, OUTPUT);  //setup LED pins
  pinMode(farPin,OUTPUT);
  pinMode(lostPin,OUTPUT);
}

void loop()
{
  boolean newData = pollGPS();  
  if (newData)
  { //if we have GPS fix
    notLost();
  }
  else
  {
    //We're lost - Show this by lighting all LED's
    digitalWrite(nearPin,HIGH);
    digitalWrite(farPin,HIGH);
    digitalWrite(lostPin,HIGH);
    // Dont know where we are, wait a little longer
    #if LOW_POWER_MODE
      Sleepy::loseSomeTime(5000);
    #else
      delay(5000);
    #endif
  }

}  

boolean pollGPS() //poll for GPS data for up to one second
{
  #if DEBUG
     Serial.print("Polling GPS: ");
  #endif
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      #if DEBUG
          Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      #endif
      if (gps.encode(c)) // Did a new valid sentence come in?
        return true;
    }
  }
  return false;
}

void notLost()
{
    lostCount=0;
    float flat,flon,dist;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);    

    dist = calc_dist( flat, flon, targetLat, targetLon);

    if (dist < nearDist)
    {
      digitalWrite(nearPin,HIGH);
      digitalWrite(farPin,LOW);
      digitalWrite(lostPin,LOW);      
    }
    else if (dist < farDist)
    {
      digitalWrite(nearPin,LOW);
      digitalWrite(farPin,HIGH);
      digitalWrite(lostPin,LOW);     
    }
    else
    {
      digitalWrite(nearPin,LOW);
      digitalWrite(farPin,LOW);
      digitalWrite(lostPin,HIGH);      
    }
#if LOW_POWER_MODE
    Sleepy::loseSomeTime(10000);
#else
    delay(10000);
#endif
}
  


/*************************************************************************
 * //Function to calculate the distance between two waypoints
 * //From: http://forum.arduino.cc/index.php/topic,27541.0.html
 *
 * // Appears to be based on the haversine formular
 *      - http://blog.avangardo.com/2013/02/distance-between-two-points-on-the-earth/
 *************************************************************************/
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
    
    // This represents the 
    dist_calc=(2*atan2(sqrt(dist_calc),sqrt(1.0-dist_calc)));
    
    dist_calc*=6371000.0; //Converting to meters

    #if DEBUG
       Serial.print("Distance to target:");
       Serial.println(dist_calc);
    #endif

    return dist_calc;
}

