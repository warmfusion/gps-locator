#include <SoftwareSerial.h>
#include <bv4618_I.h>
#include <Wire.h>

#include <TinyGPS.h> //include TinyGPS library
#define DEBUG 1           //Serial logging at the price of more memory consumption?


#include <JeeLib.h> // Low power functions library
ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog


// Declare functions
void notLost();
boolean pollGPS();
float calc_dist(float, float, float, float);
// End of Function declaration

TinyGPS gps; //initialise GPS object
SoftwareSerial ss(4, 3); // Create pseudo serial on 4:rx, 3:tx so we can still debug
//LCD Screen
BV4618_I di(0x31); // 0x62 I2C address (8 bit)

float dist = 5000;
float targetLat=51.370578, targetLon=-2.384146;
float nearDist = 25, farDist = 100;

void setup()
{
  #if DEBUG
    Serial.begin(9600);      //initialise serial port
  #endif

  //Prepare the software serial for the GPS
  ss.begin(9600); // GPS module transmits at 9600 baud
  // Setup LCD Display
  di.setdisplay(4,20);
  di.backlight(1);
  di.cls();

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
    //Oh no! We're lost :-(
    di.cls();
    di.rowcol(1,1);  
    di.puts("Finding Location...");
    
    // Dont know where we are, wait a little longer
    Sleepy::loseSomeTime(5000);
  }
    
  Sleepy::loseSomeTime(1000);
}  

boolean pollGPS() //poll for GPS data for up to one second
{
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      #if DEBUG
//          Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      #endif
      if (gps.encode(c)){ // Did a new valid sentence come in?
        #if DEBUG
          float flat, flon;
          unsigned long age;
          gps.f_get_position(&flat, &flon, &age);
          Serial.println();
          Serial.println("Location found...");
          Serial.print("LAT=");
          Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
          Serial.print(" LON=");
          Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
          Serial.print(" SAT=");
          Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
          Serial.print(" PREC=");
          Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
          Serial.println();
        #endif
        return true;
      }
    }
  }
  return false;  
}

void notLost()
{
    float flat,flon,newDist;
    char buf[20];
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);    

    // Display Lattitude
    di.rowcol(1,1);  
    // sprintf float functions don't get auto linked, so work around that fact to create equivalent of 0.6f...
    
// The following bit of code tries to use dtostrf to make the floating point to string conversion,
// but it adds 2k to the compile size which is frankly ridiculous for a bit of display
//   char lcdLine[21] = "Lat :"; //array for LCDLine
//    dtostrf(flat, 10, 6, &lcdLine[6]);
//    di.puts(lcdLine);


    // Display Latitude
    di.rowcol(1,1);
    // sprintf float functions don't get auto linked, so work around that fact to create equivalent of 0.5f...
    // WARNING: anything larger than 10000 seems to create floating point errors :-(
    int flat1 = (flat - (int)flat) * 10000;
    sprintf(buf,"Lat: %0d.%d", (int)flat, abs(flat1)); 
    di.puts(buf);
    di.clright();
    
    // Display Longitude 
    di.rowcol(2,1);
    // sprintf float functions don't get auto linked, so work around that fact to create equivalent of 0.6f...
    int flon1 = (flon - (int)flon) * 10000;
    sprintf(buf,"Lon: %0d.%d", (int)flon, abs(flon1)); 
    di.puts(buf);
    di.clright();
    
    dist = TinyGPS::distance_between( flat, flon, targetLat, targetLon);
    
    // Display Distance to target
    di.rowcol(4,1);
    sprintf(buf, "Head %dm, %s       ", int(dist), TinyGPS::cardinal(TinyGPS::course_to(flat, flon, targetLat, targetLon)) );
    di.puts(buf);
    di.clright();

    #if DEBUG
       Serial.print("Distance to target (m) : "); Serial.print(dist);
    #endif    

    if (dist < nearDist)
    {
      di.rowcol(3,1);
      di.puts("Hot hot hot!");      
      di.clright();
    }
    else if (dist < farDist)
    {
      di.rowcol(3,1);
      di.puts("Getting Warmer");
      di.clright();
    }
    else
    {
      di.rowcol(3,1);
      di.puts("Cold....");
      di.clright();
    }
    
    // Return to home
    di.crhome();

}

