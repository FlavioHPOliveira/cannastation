/*
  Esp8266 Websockets Client
  This sketch:
        1. Connects to a WiFi network
        2. Connects to a Websockets server
        3. Sends the websockets server a message ("Hello Server")
        4. Prints all incoming messages while the connection is open
  Hardware:
        For this sketch you only need an ESP8266 board.
  Created 15/02/2019
  By Gil Maimon
  https://github.com/gilmaimon/ArduinoWebsockets
*/

#include <ArduinoWebsockets.h>
//JSON main library
//https://github.com/arduino-libraries/Arduino_JSON/blob/master/examples/JSONObject/JSONObject.ino
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <string>


/* Wifi connection */
const char* ssid = "2G_Netvirtua80"; //Enter SSID
const char* password = "34424595"; //Enter Password

/*Web Socket*/
const char* websockets_server_host = "192.168.0.213"; //Enter server adress
const uint16_t websockets_server_port = 3000; // Enter server port
using namespace websockets;
WebsocketsClient client;

/*Sensors*/
//#define DHTPIN 5             // Digital pin connected to the DHT sensor. Code = 5, ESP8266 D1.
#define DHTPIN 14             // Digital pin connected to the DHT sensor. Code = 14, ESP8266 D5.
#define DHTTYPE DHT22        // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);
int soilPIN  = A0;           //Connect the soilMoisture output to analogue pin 1.
//DEFINE the VARIABLES for AIR and water to calibrate the soil mositure sensor
const int aire = 786;
const int agua = 377;

/*Flag to send sensor data, only sends data when user is accessing the board control page*/
int flagSendSensor = 0;

void setup() {

    // TESTING signals with led Initialize the LED_BUILTIN pin as an output.
    //pinMode(LED_BUILTIN, OUTPUT);     
  
    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // Check if connected to wifi
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("No Wifi!");
        return;
    }

    Serial.println("Connected to Wifi, Connecting to server.");
    // try to connect to Websockets server
    bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    //bool connected = client.connect(websockets_connection_string);
    if(connected) {
        Serial.println("Connecetd!");
        client.send("Hello Server");
    } else {
        Serial.println("Not Connected!");
    }
    
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message) {
      
        Serial.print("Got Message: ");
        Serial.println(message.data());

        JSONVar messageJSON = JSON.parse(message.data());
        if (JSON.typeof(messageJSON) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }

        //Serial.println((const char*) messageJSON["type"]);
        String messageType = (const char*) messageJSON["type"];

        //If it is a message to update the control(output)...
        if( messageType == "control"){

          String control = (const char*) messageJSON["control"];
          int onOff = (int) messageJSON["OnOff"];
          int GPIO = (int) (int) messageJSON["GPIO"];

          Serial.println("inside if is control");
          Serial.print("Control:");
          Serial.println(control);
          Serial.print("Turn On or Off:");
          Serial.println(onOff);

          //Keyword LOW turns it ON. keyword HIGH turns it Off.
          //OnOff is 0 or 1. 1 is checked, 0 is not checked.
          //0 TURNS IT ON, 1 TURNS IT OFF. I am sending it inverted from the application front end..not sure the best way.
          pinMode(GPIO, OUTPUT);
          digitalWrite(GPIO, onOff);
          
//          if( onOff == 1 ){
//            Serial.print("turn lights:");
//            pinMode(GPIO, OUTPUT);
//            digitalWrite(GPIO, LOW);
//          }
//          else if( onOff == 0 ){
//            Serial.println("turn lights OFF");
//            pinMode(GPIO, OUTPUT);
//            digitalWrite(GPIO, HIGH);
//          }   
//          

        }// end if message to update control
        
        //atoi doesnt work to convert from JSONVar to INT!!! becareful, in tthe other board it used to work....
        //int value = atoi(controlInfo[keys[1]]);        
        
    }); //end of client callback

    /*Initialize temperature and humidity sensor.*/
    dht.begin();
}

void loop() {
    // let the websockets client check for incoming messages
    if(client.available()) {
        client.poll();
        // Serial.println("Client available after pool");
        String temperature = String(dht.readTemperature());
        String airHumidity    = String(dht.readHumidity());
 
        int soilMoistureInt = map(analogRead(soilPIN), aire, agua, 0, 100);
        String soilMoistureString = String(soilMoistureInt);

        String JSONType = "{\"type\":\"sensor\"";
        String sensorJSONTemperature = "\"temperature\":\""+temperature+"\"";
        String sensorJSONAirHumidity = "\"airHumidity\":\""+airHumidity+"\"";
        String sensorJSONSoilMoisture = "\"soilMoisture\":\""+soilMoistureString+"\"}";
        
        String sensorJSONConcat = JSONType + ","+ sensorJSONTemperature + "," +sensorJSONAirHumidity + "," + sensorJSONSoilMoisture;
        //char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
        
        //Serial.println(sensorJSONConcat);
        //DynamicJsonDocument doc(1024);
        //doc["temperature"]  = dht_temperature;
        //doc["air_humidity"] = dht_humidity;
        //String sensorSerialized = doc;
        //deserializeJson(doc, json);
        
        client.send(sensorJSONConcat);

        //Test this later
        /// https://github.com/gilmaimon/ArduinoWebsockets/issues/75
        //      client.send(JSON.stringify(myObject));
        // Serial.print(JSON.stringify(myObject));
        // Serial.println("Got a 
        
    }else {
        Serial.println("Client not available: ");
        Serial.println("Reseting client object...");
        client = {}; // This will reset the client object
        // try to REconnect to Websockets server
        bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
        //bool connected = client.connect(websockets_connection_string);
        if(connected) {
            Serial.println("REConnecetd!");
            // client.send("Hello Server, we are back online"); doesnt send messages
        } else {
            Serial.println("Not REconnected!");
        }      
    }
    
    delay(500);
}
