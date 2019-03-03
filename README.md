# Arduino-Plant-webserver
An Arduino/ESP8266 based webserver to watch over a plant
Hello! 
Here is a small stand alone plant watching application I built for fun!
An Arduino Nano is equipped with:
  
      A DHT22 Temperature/Humidity sensor
      
      A Soil Hygrometer
      
      A photoresistor
      
      And an ESP8266 Webserver

The project is stand alone meaning that no external web resources are being used.
It can be set up using just a router.

The ESP8266 holds the webserver along with the HTML/CSS/Javascript code 
    the web app is updated via AJAX calls with an XML file containing the plant's current data
    The module recieves the plant data via soft serial communication from the Arduino.
    
The Arduino takes information from all the sensors connected to it continously.
  it sends the data to the ESP module for proper updates.
 
I did not draw out the schematic as I was doing it for fun and time is limited,
but here is the basic breakdown of how the circuit is connected:

ARDUINO:
    DHT22--> D7,VCC(5V), GND
    
    Hygrometer--> A0, VCC (3.3V), GND
    
    PhotoResistor--> A0, VCC (5V), GND
    
    ESP8266 --> 3.3V (from AMS117 voltage regulator), GND, D10 (RX) D11 (TX)
    
ESP8266:
    connected in the normal way:
    
      TX--> D11 (arduino) **TX for programming mode
      
      RX--> D10 (arduino) **RX for programming mode
      
      3.3V -->AMS117 (5V pin from arduino)
      
      Ch_PD --> AMS117 3.3V
      
      GND--> GND (common)
      
All Hardware required for this project:

      Arduino Nano
      
      ESP8266 + breakout board (not required but helpful)
      
      Hygrometer + breakout board
      
      DHT22 breakout board
      
      PhotoResistor (breakout board)
      
      AMS117 3.3V regulator
      
      Jumper wires
      
      Soldering iron/Solder
      
      Battery pack/power source
      
      Home router and admin access (to open the webapp)
      
 
 Some stuff to do in the future (if time permits)
 
      add some data management system (it currently does not save any data)
 
      Improve Analytics 
      
      --> currently there are a few if statements 
            that convert the raw input to something 
            more readable by a human, but as with everything it can be improved.
            
      Implement websockets instead of AJAX polling
      
 If you have any suggestions, questions, comments or concerns please feel free to let me know!
 Thanks for looking!
 -Caleb
