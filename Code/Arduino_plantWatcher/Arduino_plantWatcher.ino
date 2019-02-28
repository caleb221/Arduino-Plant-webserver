/*===============================================
 * I made a small standalone IoT device! 
 *  This is the code for the ARDUINO
 *  Sensors:
 *      DHT22 (Digital pin 7) 5V
 *      Photo resistor (Analog Pin 5) 5V
 *      Soil hygrometer (Analog Pin 0) 3.3V
 *  Server:  
 *      ESP8266-01 (Rx - 10/ Tx -11) 5v Arduino --> 3.3V with AMS117 Regulator
 *      -->Server code is elsewhere
 ****************************************  
 *  Special thanks to all those helpful
 *  people on the internet who found
 *  solutions to all my problems
 ****************************************  
 * -Caleb Seifert
 *===============================================  
*/

//Libraries
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "SoftwareSerial.h"

//Constants
#define DHTPIN 7     
#define DHTTYPE DHT22  
#define hygroPin A0
#define photoPin A5

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial esp(10,11);//RX / TX


//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value

int hydroLevel;
int lightLevel;
String light,wet,sendData1;
void setup()
{
  //Serial.begin(115200);
  dht.begin();
 // Serial.println("hi");
  esp.begin(115200);
}

void loop()
{
    //delay(2000);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    hydroLevel=analogRead(hygroPin);
    lightLevel=analogRead(photoPin);//above 800 is very bright
    //format input data  
    if(hydroLevel > 670)
    {
      wet="d";//dry
    }
    if(hydroLevel < 355)
    {
      wet="w";//wet
    }
   
    if(lightLevel<20)
    {
     light="d"; //dark
     }
     if(lightLevel>20 && lightLevel<180)
    {
     light="m"; //medium
     }
    if(lightLevel>180)
    {
     light="b"; //bright
     }
           // above 500 is bright
          // below 200 is dim
         //  below 10 is very dark
//pack the data into a single string... '>' is ending character 
sendData1 = String(temp)+":"+String(hum)+":"+wet+":"+light+">";
//send it to ESP
updateData(sendData1);
delay(2000);
    //delay(10000); //Delay 2 sec.
}

//format string to char and send off through the wire!
void updateData(String outBound)
{ 
 char* cString = (char*) malloc(sizeof(char)*(outBound.length() + 1));
  for(int i=0;i<outBound.length()+1;i++)
  {
    cString[i]=sendData1[i];  
  }
  //Serial.println(esp.available());
  if(esp.available()>0)
  {
      esp.write(cString);
      delay(5);
  }
}
