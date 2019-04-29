/*===============================================
 * This is the base code for a NodeMCU 
 *  standalone IoT device!
 * If you want to be able to water a plant 
 * from your phone/computer/any connected device, here is the software!
 * --> in addition to watering your plant, you are also able to see:
 *     Temperature
 *     Humiditity
 *     the % of water in the plant's soil
 *     --> ALSO there is code for a 5V water storage detector commented out
 * The NodeMCU is using the websocket protocol for bi-directional communication
 *     between you (your computer/phone) and the microcontroller
 *     the data is packaged as JSON, should you wish to keep a log its already
 *     in a usable format.
 * The circuit should be pretty easy to build according to the code.
 * the soil hygrometer is connected to A0
 * the DHT22 temp/humidity sensor is on D2
 * the water detection sensor is on pin D5
 * the small 5V DC water pump is connected to pin D7
 * --> please note the water pump is connected to a PN2222 transistor
 *     and external power source because my batteries were not large
 *     but given a larger battery it should be fine
 * the NodeMCU is given 3.3V through an AMS117 voltage regulator
 *****************************************  
 *  Special thanks to all those helpful
 *  people on the internet who found
 *  solutions to all my problems
 ****************************************  
 * -Caleb Seifert
 *=============================================== 
 */


#include<WebSocketsServer.h>
#include<ESP8266WiFi.h>
#include<ESP8266WebServer.h>
#include<WiFiClient.h>
#include<ArduinoJson.h>
#include<DHT.h>
#include<Adafruit_Sensor.h>
//=====================
//  PINS
#define hygroPin A0 //3.3v
#define dhtPin D2 //5v
#define dhtType DHT22 
//#define foodPin D5  //5v waterDetector
#define feederPin D7 //motorPin  5//5v
//=====================

//=====================
// VIN OUTPUTS 5v!!!??
//=====================

//===========================
//      socket stuff
WebSocketsServer socket= WebSocketsServer(666);//port: 666
ESP8266WebServer server(80);
String jsonData;
// SocketVariables are sent to client every 50 milliseconds

//============================
//    Sensor stuff
DHT dht(dhtPin,dhtType);
float hydroLevel=0;
float humid=0;
float temp =0;
//int isFood=0;
//===========================
//      Server stuff
const char* ssid="Steal_your_cookies";
const char* passw="password";
String html,javaScript,css;
//===========================

void setup() {

  dht.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,passw);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("");
  Serial.println(WiFi.localIP());
  Serial.println("setup...WHERE THE DATA?");
  

server.on("/",makeWebsite);
socket.begin();
socket.onEvent(socketFunction);
server.onNotFound(handleNotFound);
server.begin();
//pinMode(foodPin,INPUT);
pinMode(feederPin,OUTPUT);

}

void loop() {

socket.loop();
server.handleClient();
checkSensors();
jsonData=makeJson();
socket.broadcastTXT(jsonData);
}


void socketFunction(uint8_t num, WStype_t type, uint8_t * payload, size_t plength)
{
    String payloadString=(const char *)payload;
  if(type == WStype_TEXT)
  {
     //call watering function here
     //Serial.print(payloadString);
      if(payloadString == "GO")
      {
        waterPlant(true); 
      }
      else
      {
       waterPlant(false); 
      }
  }
}

 void waterPlant(boolean fd)
 {
  if(fd)
  {
    digitalWrite(feederPin,HIGH);
    Serial.println("WATERING....\n");
    delay(5000);
  }
  else
  {
    digitalWrite(feederPin,LOW);
    Serial.println("NOT WATERING....");
  }
 }


void makeWebsite()
{
  makeJscript();
  makeStyle();
  html="<!DOCTYPE html>";
  html+="<head>";
  html+="<meta name='viewport' content='width=device-width, initial-scale=1' /> ";
  html+="<meta charset='utf-8'>";
  html+="<style>";
  html+=css;
  html+="</style>";
  html+="</head>";
  html+="<body onload='javascript:start()'>";
  html+="<h1>Dat Node Farm V2</h1>";
  html+="<div class='leaf'> ";
  html+="<div class='dataFlow' id = 'temp'>";
  html+="Temp</div></div>";
  html+="<p class='dataLabel'>";
  html+="Temperature</p>";
  html+="<br><br>";
  html+="<div class='leaf'>";
  html+="<div class='dataFlow' id='humid'>100%</div>";
  html+=" </div>";
  html+="<p class='dataLabel'>Humidity</p>";
  html+="<br><br>";
  html+="<div class='leaf'>";
  html+="<div class='dataFlow' id='waterLev'>";
  html+="100%</div> </div>";
  html+="<p class='dataLabel'>Soil Water (%)</p>";
  html+="<br><br>";
  /*
  html+="<div class='leaf'>";
  html+="<div class='dataFlow' id='storage'>";
  html+="Full</div> </div>";
  html+="<p class='dataLabel'>Food Storage</p>";
  html+="<br>";
  */
  html+="<input class='leaf' type='button' id='waterPlant' onclick='waterPlant()' value='Water!' >";
  html+="<br><br>";
  html+=" <input  class ='leaf' type='button' onclick='location.href=location.href' value='Refresh!'>";
  html+="<br> <br> <br> <br> <br> <br> <br>";
  html+="<script>";
  html+=javaScript;
  html+="</script>";
  html+="</body>";
  html+="</html>";
  server.send(200,"text/html",html);
}

void makeJscript()
{
  javaScript+="var wBtn = document.querySelector('#waterPlant');\n";
  javaScript+="var socket;";
  javaScript+="function start(){\n var loc = window.location.hostname;";
  javaScript+="socket = new WebSocket('ws://'+loc+':666/');\n "; 
  javaScript+="console.log(socket);\n";
  javaScript+="socket.onmessage = function(evt){\n";  
  javaScript+="var data = JSON.parse(evt.data);\n";
  javaScript+="//console.log(data);\n";
  javaScript+="updateApp(data);\n";
  javaScript+="};";
  javaScript+="}\n";
  javaScript+="function updateApp(data){\n";
  javaScript+="var humid=document.querySelector('#humid'); \n";
  javaScript+="var waterLev =document.querySelector('#waterLev');\n";
  javaScript+="var temp=document.querySelector('#temp');\n";
  javaScript+="var foodStore=document.querySelector('#storage');\n";
  javaScript+="temp.innerHTML=data.temp;\n";
  javaScript+="humid.innerHTML=data.humidity;\n";
  javaScript+="waterLev.innerHTML=data.soilWater;\n";
 /*
  javaScript+="if(data.waterSupply == 0) { \n";  
  javaScript+="foodStore.innerHTML='没有';\n";
  javaScript+="} else { \n";    
  javaScript+="foodStore.innerHTML='有'; } \n";
  */
  javaScript+="}\n";
  javaScript+="function waterPlant() { \n";
  javaScript+="var delayTime = 1000*5;\n";
  javaScript+="var goingOut='GO';\n";
  javaScript+="socket.send(goingOut);\n";
  javaScript+="wBtn.disabled=true;\n";
  javaScript+="setInterval(stopTimer,delayTime);\n";
  javaScript+="goingOut='STOP';\n";
  javaScript+="socket.send(goingOut);\n";
  javaScript+="}\n";
  javaScript+="function stopTimer() { \n";
  javaScript+="clearInterval(50);\n";
  javaScript+="wBtn.disabled=false; \n";
  javaScript+="}\n";
}
void makeStyle()
{  
  css="@media screen and (max-width: 1020px) {";
  css+="#container, #header, #content, #footer{";
  css+="float: none; width: auto; }";
  css+=" body{";
  css+="background-image: linear-gradient(powderblue,darkgreen);";
  css+="font-family: monospace;";
  css+="color:cornsilk;";
  css+="background-color: darkgreen;";
  css+="background-repeat: no-repeat;";
  css+=" }";
  css+="h1{ color: black; }";
  css+=".leaf {";
  css+="float: left; width: 95px; height: 70px; background-color: #A0DE21; ";
  css+=" -moz-border-radius: 100px 0px; -webkit-border-radius: 100px 0px;";
  css+=" border-radius: 100px 0px; border-image-slice: 5; text-align: center;";
  css+=" text-indent: inherit; }";
  css+=".dataFlow {";
  css+="  margin: 20px; color:black; font-size: 16px"; 
  css+=" }";
  css+=".dataLabel { padding-left:80px; padding-top: 15px; }";
  css+=" }";

 
}

 //I WANNA MAKE A JSON FILE...SEND IT TO THE PISERVER FOR DATA COLLECTION
 //ANALYSIS/LOGGING
void checkSensors()
{
  
  temp=dht.readTemperature();
  humid=dht.readHumidity();
  hydroLevel=analogRead(hygroPin);
 // isFood = digitalRead(foodPin);
if(isnan(temp) || isnan(humid))
{
  Serial.println("DHT READ FAIL...");
  delay(1000);
  temp=404;
  humid=404;
  //checkSensors();
}
/*
  Serial.println("temp");
  Serial.print(temp);
  Serial.println("\nHumid:");
  Serial.print(humid);
  Serial.println("\nWater:");
*/
//Serial.print(hydroLevel);
  hydroLevel=map(hydroLevel,1024,350,0,100);
  //Serial.print(hydroLevel);
  //Serial.print("\n");
  delay(50);
 }

String makeJson()
{
  checkSensors();
  String buff;
   DynamicJsonBuffer jBuffer;
   JsonObject& root = jBuffer.createObject();
   root["temp"]=temp;
   root["humidity"]=humid;
   root["soilWater"]=hydroLevel;
 //root["waterSupply"]=isFood;
   root.printTo(buff);
   return buff;  
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
