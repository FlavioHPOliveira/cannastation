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
  https://github.com/gilmaimon/ArduinoWebsockets
*/
#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <ArduinoWebsockets.h>
//JSON main library
//https://github.com/arduino-libraries/Arduino_JSON/blob/master/examples/JSONObject/JSONObject.ino
#include <Arduino_JSON.h>
#include <ArduinoJson.h> // to deal with JSON FS stuff
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <string>

//WifiManager
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//Wifi Manager configuration
//flag for saving data. (Not sure this is necessary)
bool shouldSaveConfig = false;
//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

//Get Time
#include <NTPClient.h>
#include <WiFiUdp.h>

/*Define NTP Client to get time
  https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
*/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//Week Days
//String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Month names
//String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


/* Wifi connection */
//const char* ssid = "2G_Netvirtua80"; //Enter SSID
//const char* password = "34424595"; //Enter Password

/*Web Socket*/
//const char* websockets_server_host = "192.168.0.5"; //Enter server adress
//const uint16_t websockets_server_port = 3000; // Enter server port
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

/*Flag to send sensor data, only sends data when user is accessing the board control page.
  NOT BEING USED SO FAR...*/
int flagSendSensor = 0;

/* Board token that will be populated when user signs in to Wifi */
char tokenChar[40];
String token = "initToken";
String connectionURL = "InitURL";

/*GPIO variables*/
int GPIO_LIGHT   = 5;
int GPIO_FAN     = 4;
int GPIO_EXHAUST = 0;
int GPIO_WATER   = 2;

//Light Control Variables
int lightAuto       = 0;
int lightOn         = 0;
int lightHourOn     = 17;
int lightMinuteOn   = 52;
int lightHourOf     = 17;
int lightMinuteOff  = 53;

void setup() {

  Serial.begin(115200);

  /////////////////////////////READ CONFIGURATION FROM FS JSON///////////////////////////////

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if ( ! deserializeError ) {
#else
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
#endif
          Serial.println("\nparsed json");
          strcpy(tokenChar, json["tokenChar"]);
//          strcpy(mqtt_port, json["mqtt_port"]);
//          strcpy(api_token, json["api_token"]);
        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  /////////////////////////////END OF READ CONFIGURATION FROM FS JSON///////////////////////////////

  /////////////////////////////WIFI MANAGER SET UP AND CONNECT///////////////////////////////
  WiFiManagerParameter custom_tokenChar("tokenChar", "Station Token", tokenChar, 40);

  WiFiManager wifiManager;
  
  //set config save notify callback (is there a case where I dont want it to be saved? not sure this is necessary..)
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  //wifiManager.autoConnect("Cannastation");

  //add all your parameters here
  wifiManager.addParameter(&custom_tokenChar);
  //wifiManager.addParameter(&custom_mqtt_port);
  //reset settings - for testing
  //wifiManager.resetSettings();

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("Cannastation")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  Serial.println("connected to wifi...yeey :)");
  
  /////////////////////////////END OF WIFI MANAGER SET UP AND CONNECT///////////////////////////////

  /////////////////////////////SAVING WIFI MANAGER PARATEMERS TO THE BOARD FS///////////////////////////////
    //read updated parameters
  strcpy(tokenChar, custom_tokenChar.getValue());
//  strcpy(mqtt_port, custom_mqtt_port.getValue());
//  strcpy(api_token, custom_api_token.getValue());
  Serial.println("The values in the file are: ");
  Serial.println("\tokenChar : " + String(tokenChar));
//  Serial.println("\tmqtt_port : " + String(mqtt_port));
//  Serial.println("\tapi_token : " + String(api_token));

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
#endif
    json["tokenChar"] = tokenChar;
//    json["mqtt_port"] = mqtt_port;
//    json["api_token"] = api_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, Serial);
    serializeJson(json, configFile);
#else
    json.printTo(Serial);
    json.printTo(configFile);
#endif
    configFile.close();
    //end save
  }

  /////////////////////////////END OF SAVING WIFI MANAGER PARATEMERS TO THE BOARD FS///////////////////////////////

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example: GMT +1 = 3600
  timeClient.setTimeOffset( -3600 * 3 ); //Brazilian time GMT -3

  // TESTING signals with led Initialize the LED_BUILTIN pin as an output.
  //pinMode(LED_BUILTIN, OUTPUT);

  // Connect to wifi
  //WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  //for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
  //    Serial.print(".");
  //    delay(1000);
  //}

  // Check if connected to wifi
  //    if(WiFi.status() != WL_CONNECTED) {
  //        Serial.println("No Wifi!");
  //        return;
  //    }

  //    Serial.print("Connected to Wifi, Connecting to server: ");
  //    Serial.println(connectionURL);
  //
  // try to connect to Websockets server
  //bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  token = tokenChar;
  Serial.println(token);
  connectionURL = "ws://192.168.0.213:3000/?token=" + token + "?clientType=board";
  Serial.println(connectionURL);
  bool connected = client.connect(connectionURL);
  if (connected) {
    Serial.println("Connecetd to WS!");
    client.send("Hello Server");
  } else {
    Serial.println("Not Connected to WS!");
  }

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) {

    Serial.print("(serialPrint)Got Message: ");
    Serial.println(message.data());

    JSONVar messageJSON = JSON.parse(message.data());
    if (JSON.typeof(messageJSON) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    //Serial.println((const char*) messageJSON["type"]);
    String messageType = (const char*) messageJSON["type"];

    //If it is a message to update the control(output)...
    if ( messageType == "control") {

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
    }// ENDIF MANUAL update

    if ( messageType == "control_auto") {

      String control = (const char*) messageJSON["control"];
      lightAuto       = 1;
      lightHourOn     = (int) messageJSON["hourOn"];
      lightMinuteOn   = (int) messageJSON["minuteOn"];
      lightHourOf     = (int) messageJSON["hourOff"];
      lightMinuteOff  = (int) messageJSON["minuteOff"];

      Serial.print("New Light ON Hour: ");
      Serial.println(lightHourOn);
      Serial.println(lightMinuteOn);
      Serial.println(lightHourOf);
      Serial.println(lightMinuteOff);

      //TODO
      //save settings on file.

    }// ENDIF AUTOMATIC update

    //atoi doesnt work to convert from JSONVar to INT!!! becareful, in tthe other board it used to work....
    //int value = atoi(controlInfo[keys[1]]);

  }); //end of client callback

  /*Initialize temperature and humidity sensor.*/
  dht.begin();
}

void loop() {
  
  //Verify if there is wifi connection and tries to reconnect.
//  if (!wifiManager.autoConnect("Cannastation")) {
//    Serial.println("failed to connect and hit timeout");
//    delay(3000);
//    //reset and try again, or maybe put it to deep sleep
//    ESP.reset();
//    delay(5000);
//  }
//  if (WiFi.status() == WL_CONNECTED) {
//      Serial.println("WIFI connected again!");
//    }
//  else {
//    // Connection is just lost
//    ESP.restart();
//  }

  timeClient.update();
  //    unsigned long epochTime = timeClient.getEpochTime();
  //    String formattedTime = timeClient.getFormattedTime();
  //    String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);
  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute);
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);
  //
  if ( lightAuto == 1 )
  {
    if (lightOn == 0) {
      if ( currentHour == lightHourOn && currentMinute == lightMinuteOn) {
        lightOn = 1;
        pinMode(GPIO_LIGHT, OUTPUT);
        digitalWrite(GPIO_LIGHT, LOW); //LOW turns it ON, HIGH turns it OFF
      }
    }
    else if (lightOn == 1) {
      if ( currentHour == lightHourOf && currentMinute == lightMinuteOff) {
        lightOn = 0;
        pinMode(GPIO_LIGHT, OUTPUT);
        digitalWrite(GPIO_LIGHT, HIGH); //LOW turns it ON, HIGH turns it OFF
      }
    }
  }

  // let the websockets client check for incoming messages
  if (client.available()) {
    client.poll();
    // Serial.println("Client available after pool");
    String temperature = String(dht.readTemperature());
    String airHumidity    = String(dht.readHumidity());

    int soilMoistureInt = map(analogRead(soilPIN), aire, agua, 0, 100);
    String soilMoistureString = String(soilMoistureInt);

    String JSONType = "{\"type\":\"sensor\"";
    String sensorJSONTemperature = "\"temperature\":\"" + temperature + "\"";
    String sensorJSONAirHumidity = "\"airHumidity\":\"" + airHumidity + "\"";
    String sensorJSONSoilMoisture = "\"soilMoisture\":\"" + soilMoistureString + "\"}";

    String sensorJSONConcat = JSONType + "," + sensorJSONTemperature + "," + sensorJSONAirHumidity + "," + sensorJSONSoilMoisture;
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

  } else {
    Serial.println("Client not available: ");
    Serial.println("Reseting client object...");
    client = {}; // This will reset the client object
    // try to REconnect to Websockets server
    //        bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    bool connected = client.connect(connectionURL);
    //bool connected = client.connect(websockets_connection_string);
    if (connected) {
      Serial.println("REConnecetd!");
      // client.send("Hello Server, we are back online"); doesnt send messages
    } else {
      Serial.println("Not REconnected!");
    }
  }

  delay(1000);
}
