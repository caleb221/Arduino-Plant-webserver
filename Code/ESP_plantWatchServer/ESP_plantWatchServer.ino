/*===============================================
 * I made a small standalone IoT device! 
 *  This is the code for the ESP8266-01
 *  Sensors:
 *      DHT22 (Digital pin 7) 5V
 *      Photo resistor (Analog Pin 5) 5V
 *      Soil hygrometer (Analog Pin 0) 3.3V
 *  Server:  
 *      ESP8266-01 (Rx - 10/ Tx -11) 5v Arduino --> 3.3V with AMS117 Regulator
 *-->Arduino code is elsewhere
 ****************************************  
 *  Special thanks to all those helpful
 *  people on the internet who found
 *  solutions to all my problems
 ****************************************  
 * -Caleb Seifert
 *===============================================  
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#define INPUT_SIZE 16 //15 for my string..woohoo! its small!

ESP8266WebServer server(80); // 80 is the port number
//=====================================================


const char *ssid = "********";
const char *password = "password";
int hydroLevel=0;
int lightLevel;
String js,XML,website,css;
String hum="0",temp="0",water,light;

boolean newData=false;
const byte dataLen=32;
char dataIn[dataLen];

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  server.on("/", websiteContent);
  server.on("/xml", XMLcontent);
  server.onNotFound(handleNotFound);
  server.begin();
  //webSocket.begin();
 // webSocket.onEvent(webSocketEvent);
  //websockCount = 1;
  Serial.println("HTTP server started");
}

void loop() { 

incomingData();
showNewData();
server.handleClient();

}
void incomingData()
{

static byte i=0;
char endMark='>';
char rc;

while(Serial.available()>0 && newData == false)
  {
   rc=Serial.read();
   if(rc != endMark)
   {
     dataIn[i]=rc;
     i++;
     if(i>=dataLen)
     {
       i=dataLen-1;  
     }
   }
   else{
     dataIn[i]='\0';//kill it
     i=0;
     newData=true;
   }//endelse
 }//endwhile  
  
}

void showNewData() {
    if (newData == true) 
    {    
       //string splitting
       temp= getValue(dataIn, ':', 0);
       hum= getValue(dataIn, ':', 1);
       water= getValue(dataIn, ':', 2);
       
       light= getValue(dataIn, ':', 3);
       if(water.equals("w"))
       {
        water="Fed"; 
       }
       if(water.equals("d"))
       {
        water="Hungry";
        }
        //==water==
       if(light.equals("d"))
        {
        light ="Dark";
        }
        if(light.equals("m"))
        {
        light ="Middle";
        }
        if(light.equals("b"))
        {
        light ="Sunny";
        }
        newData = false;
    }
    delay(15);
}


void websiteContent()
{
  Serial.print("SENDING WEBSITE!");
  JS();
  CSS();
  website="<!DOCTYPE html>";
  website+="<html>";
  website+="<head>";
  website+="<meta name='viewport' content='width=device-width, initial-scale=1' />";
  website+="<meta charset='utf-8' />";
  website+=css;//make it pretty!
  website+="</head><body onload='process()'>";
  website+="<h1>hows the plant?</h1>";
  website+="<div class='leaf'><div class='dataFlow' id='humid'>100%</div></div>";
  website+="<p class='dataLabel'>Humidity</p><br><br>";
  website+="<div class='leaf'><div class='dataFlow' id='water'>NULL</div></div>";
  website+="<p class=dataLabel>Feed the plant?</p><br>";
  website+="<div class='leaf'><div class='dataFlow' id = 'temp'>Temp</div></div>";
  website+="<p class='dataLabel'>Temperature</p><br>";
  website+="<div class='leaf'><div class='dataFlow' id='light'>Medium</div></div>";
  website+="<p class='dataLabel'>Light Level</p><br>";
  website+="<input  class ='leaf' type='button' onclick='location.href=location.href' value='Refresh!'>";
  website+=js;//functional!
  website+="</body>";
  
  website+="</html>"; 
  server.send(200,"text/html",website); 
 }

void JS()
{   
  js="<script>\n";
  js+="var xmlHttp=createXmlHttpObject();\n";
  js+="function createXmlHttpObject(){ \n";
  js+="if(window.XMLHttpRequest){ \n";
  js+="xmlHttp=new XMLHttpRequest();\n";
  js+="}else{ \n";
  js+="xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  js+="}\n";
  js+="return xmlHttp;\n";
  js+="}\n";
  js+="function response(){\n";
  js+="var t,h,w,l,xmlResponse;\n";
  js+="var tempData,humidData,waterData,lightData;\n";
  js+="xmlResponse=xmlHttp.responseXML;\n";
  js+="t = xmlResponse.getElementsByTagName('temp');\n";
  js+="h =xmlResponse.getElementsByTagName('humid');\n";
  js+="w=xmlResponse.getElementsByTagName('water');\n";
  js+="l=xmlResponse.getElementsByTagName('light');\n";
  js+="tempData = t[0].firstChild.nodeValue;\n";
  js+="humidData = h[0].firstChild.nodeValue;\n";
  js+="waterData = w[0].firstChild.nodeValue; \n";
  js+="lightData = l[0].firstChild.nodeValue;\n";
  js+="document.getElementById('temp').innerHTML=tempData;\n";
  js+="document.getElementById('humid').innerHTML=humidData;\n";
  js+="document.getElementById('water').innerHTML=waterData;\n";
  js+="document.getElementById('light').innerHTML=lightData;\n";
  js+="}\n";
  js+="function process(){ \n";
  js+="xmlHttp.open('PUT','xml',true); \n";
  js+="xmlHttp.onreadystatechange=response; \n";
  js+="xmlHttp.send(null); \n";
  js+="setTimeout('process()',200);\n";//200
  js+="}\n";
  js+="</script>";
 
 }
   
void XMLcontent(){
  XML ="<?xml version='1.0' encoding='UTF-8'?>";
  XML+="<data>";
  XML+="<temp>";
  XML+=temp;
  XML+="</temp>";
  XML+="<humid>";
  XML+=hum;
  XML+="</humid>";
  XML+="<water>";
  XML+=water;
  XML+="</water>";
  XML+="<light>";
  XML+=light;
  XML+="</light>";
  XML+="</data>";
  server.send(200,"text/xml",XML);
  }
void CSS()
{
  css="<style>";
  css+="@media screen and (max-width: 1020px) {";
  css+="#container, #header, #content, #footer{";
  css+="float: none; width: auto; }";
  css+="body{ background-image: linear-gradient(indianred,darkred);";
  css+="background-repeat:no-repeat; background-color:darkred; }";
  css+=".leaf { width: 750px; height: 60px; background-color: #A0DE21; ";
  css+=" -moz-border-radius: 100px 0px; -webkit-border-radius: 100px 0px;";
  css+="border-radius: 100px 0px; border-image-slice: 5; }";
  css+=".dataFlow { margin: 20px; color:black; font-size: 15px; }";
  css+=".dataLabel { padding-left:80px; padding-top: 15px; }";
  css+="}";
  css+="blank{}"; 
  css+="body { background-image: linear-gradient(indianred,darkred);";
  css+="background-repeat:no-repeat; background-color:darkred;";
  css+="color: antiquewhite; }";
  css+=".leaf{ float: left; width: 95px; height: 70px; background-color: #A0DE21;";
  css+=" -moz-border-radius: 100px 0px; -webkit-border-radius: 100px 0px;";
  css+="border-radius: 100px 0px; border-image-slice: 5;";
  css+="text-align: center; text-indent: inherit;}";  
  css+=".dataFlow { margin: 20px; color:black; font-size: 16px}";
  css+=".dataLabel { padding-left:80px; padding-top: 15px;}";
  css+="</style>";
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void handleNotFound() {
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
