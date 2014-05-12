#include <TinyGPS.h> //include TinyGPS library
#include <JeeLib.h> // Low power functions library
int led_pin = 13;
ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog
TinyGPS gps; //initialise GPS object
float tobyLat=55.582913, tobyLon=-1.835315;
float nearDist = 0.0002, farDist = 0.001;
int nearPin = 5, farPin = 6, lostPin = 7, powerPin = 2;
int lostCount=0;

void setup()
{
  Serial.begin(9600);      //initialise serial port
  pinMode(nearPin, OUTPUT);  //setup LED pins
  pinMode(farPin,OUTPUT);
  pinMode(lostPin,OUTPUT);
  pinMode(powerPin, OUTPUT);  
}

void loop()
{
  setPower(true); //turn on power
  boolean newData = pollGPS();  
  if (newData) //if we have GPS fix
    notLost();
  else lost();
}  

boolean pollGPS() //poll for GPS data for up to one second
{
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        return true;
    }
  }
  return false;
}

void setPower(boolean power)
{
  digitalWrite(powerPin, power);
}

void lost()
{
  lostCount++;
  if (lostCount<2)
  {
    Sleepy::loseSomeTime(5000);
    return;
  }
  else if (lostCount<17)
  {
    digitalWrite(lostPin,((lostCount+1)%2));
    digitalWrite(nearPin,LOW);
    digitalWrite(farPin,LOW);
    Sleepy::loseSomeTime(5000);
    return;
  }
  while (lostCount<23)
  {
    setPower(false);
    digitalWrite(lostPin,((lostCount+1)%2));
    digitalWrite(nearPin,LOW);
    digitalWrite(farPin,LOW);
    Sleepy::loseSomeTime(5000);
    lostCount++;
  }
  lostCount=0;
}

void notLost()
{
    lostCount=0;
    float flat,flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);    
    if (((tobyLat-nearDist)<=flat && flat<=(tobyLat+nearDist)) && ((tobyLon-nearDist)<=flon && flon<=(tobyLon+nearDist)))
    {
      digitalWrite(nearPin,HIGH);
      digitalWrite(farPin,LOW);
      digitalWrite(lostPin,LOW);      
    }
    else if (((tobyLat-farDist)<flat && flat<(tobyLat+farDist)) && ((tobyLon-farDist)<flon && flon<(tobyLon+farDist)))
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
    setPower(false);
    Sleepy::loseSomeTime(10000);
}
  
