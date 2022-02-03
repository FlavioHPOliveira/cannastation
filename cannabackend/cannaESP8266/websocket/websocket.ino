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
//Get Time
#include <NTPClient.h>
#include <WiFiUdp.h>

/////////////////////////////////  VARIABLES DECLARATION  /////////////////////////////////////

//Wifi Manager configuration
//flag for saving data. (Not sure this is necessary)
bool shouldSaveConfig = false;
//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
/*Define NTP Client to get time
  https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
*/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
/* Wifi connection */
//const char* ssid = "2G_Netvirtua80"; //Enter SSID
//const char* password = "34424595"; //Enter Password
/*Web Socket*/
//const char* websockets_server_host = "192.168.0.5"; //Enter server adress
//const uint16_t websockets_server_port = 3000; // Enter server port
using namespace websockets;
//library By Gil Maimon https://github.com/gilmaimon/ArduinoWebsockets
WebsocketsClient client;
/*Sensors*/
//#define DHTPIN 5           // Digital pin connected to the DHT sensor. Code = 5, ESP8266 D1.
#define DHTPIN 14            // Digital pin connected to the DHT sensor. Code = 14, ESP8266 D5. 
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
int lightOn         = 1;  //0 is ON, 1 is OFF
int lightHourOn     = 17;
int lightMinuteOn   = 52;
int lightHourOf     = 17;
int lightMinuteOff  = 53;

//Fan Control Variables
int fanAuto       = 0;
int fanOn         = 1;
int fanTempOn     = 999;

//Exhaust Control Variables
int exhaustAuto   = 0;
int exhaustOn     = 1; 
int exhaustAirHumidityOn = 999;

//Water Control Variables
int waterAuto     = 0;
int waterOn       = 1;
int waterStartingHour = 0;
int waterEveryXHour = 0;
int waterEveryXDay = 0;
unsigned long lastWateredTime = 999; //epoch time seconds.
int lastWateredHour = 999;
int waterDurationSeconds = 0;
 
/////////////////////////////////END VARIABLES DECLARATION/////////////////////////////////////

void setup() {

  Serial.begin(115200);

  /////////////////////////////READ CONFIGURATION FROM FS JSON///////////////////////////////

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  delay(5000); //just to give me time to open serial monitor.....
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
        DynamicJsonDocument json(4096);
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
          
          lightAuto = (int) json["lightAuto"];
          lightOn   = (int) json["lightOn"];
          pinMode(GPIO_LIGHT, OUTPUT);
          digitalWrite(GPIO_LIGHT, lightOn); //LOW turns it ON, HIGH turns it OFF
          if( lightAuto == 1 ){ // If light is auto, get values
            lightHourOn     = (int) json["hourOn"];
            lightMinuteOn   = (int) json["minuteOn"];
            lightHourOf     = (int) json["hourOff"];
            lightMinuteOff  = (int) json["minuteOff"];
          }

          fanAuto = (int) json["fanAuto"];
          fanOn   = (int) json["fanOn"];
          pinMode(GPIO_FAN, OUTPUT);
          digitalWrite(GPIO_FAN, fanOn); //LOW turns it ON, HIGH turns it OFF
          if( fanAuto == 1 ){
            fanTempOn     = (int) json["fanTempOn"];
          }

          exhaustAuto = (int) json["exhaustAuto"];
          exhaustOn   = (int) json["exhaustOn"];
          pinMode(GPIO_EXHAUST, OUTPUT);
          digitalWrite(GPIO_EXHAUST, exhaustOn); //LOW turns it ON, HIGH turns it OFF
          if( exhaustAuto == 1 ){
            exhaustAirHumidityOn = (int) json["exhaustAirHumidityOn"];
          }

          waterAuto = (int) json["waterAuto"];
          waterOn   = (int) json["waterOn"];
          pinMode(GPIO_WATER, OUTPUT);
          digitalWrite(GPIO_WATER, waterOn); //LOW turns it ON, HIGH turns it OFF
          if( waterAuto == 1 ){ // If light is auto, get values
            waterStartingHour    = (int) json["waterStartingHour"];
            waterEveryXDay       = (int) json["waterEveryXDay"];
            lastWateredTime      = (int) json["lastWateredTime"];
            lastWateredHour      = (int) json["lastWateredHour"];
            waterDurationSeconds = (int) json["waterDurationSeconds"];
          }
          

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
    DynamicJsonDocument json(2048);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
#endif

    json["tokenChar"] = tokenChar;
    //Save control default values on the board.
     
    json["lightOn"]     = lightOn;
    json["lightAuto"]   = lightAuto;
    json["fanOn"]       = fanOn;
    json["fanAuto"]     = fanAuto;
    json["exhaustOn"]   = exhaustOn;
    json["exhaustAuto"] = exhaustAuto;
    json["waterOn"]     = waterOn;
    json["waterAuto"]   = waterAuto;

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

    //add all controls auto and onoff default values. otherwise it will break when trying to read the FS file without having those settings.
//    saveControlStatusFS( GPIO_LIGHT, lightOn );
//    saveControlStatusFS( GPIO_FAN, fanOn );
//    saveControlStatusFS( GPIO_EXHAUST, exhaustOn );
//    saveControlStatusFS( GPIO_WATER, waterOn );

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

  //5 seconds delay to give some time before connecting the WS. didnt work that wel...only on first time! 
  //without this the WS doesnt connect on first try, then goes to the loop reconnect. It doesnt print on serial anymore and for sensor signal to be sent need to reconnect the board to make it work, super weird.
  delay(5000);
  
  bool connected = client.connect(connectionURL);
  for(int i = 0; i < 10 && !connected; i++) {
      Serial.print("Trying to connect WS, try:");
      Serial.println(i+1);
      delay(2500);
      connected = client.connect(connectionURL);
  }
  if (connected) {
    Serial.println("Connecetd to WS!");
    client.send("Hello Server");
  } else {
    Serial.println("Could not connected to WS!");
  }

  /////////////////////////////////////////////RECEIVE WS MESSAGES////////////////////////////////////////////////////
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

     ///////////////////////////////////////////// MANUAL CONTROL UPDATE ////////////////////////////////////////////////////
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
      //save new GPIO status on the board FS.
      saveControlStatusFS(GPIO, onOff);

      //SET control variables.
     if( GPIO == GPIO_LIGHT ){
        lightAuto = 0;
        lightOn   = onOff; 
      }else if( GPIO == GPIO_FAN){
        fanAuto   = 0;
        fanOn     = onOff; 
      }else if( GPIO == GPIO_EXHAUST){
        exhaustAuto   = 0;
        exhaustOn     = onOff; 
      }else if( GPIO == GPIO_WATER){
        waterAuto   = 0;
        waterOn     = onOff; 
      }
      
    }///////////////////////////////////////////// END MANUAL CONTROL UPDATE ////////////////////////////////////////////////////

    ////////////////////////////////////////////// AUTOMATIC CONTROL UPDATE ////////////////////////////////////////////////////
    if ( messageType == "control_auto") {

      String control = (const char*) messageJSON["control"];

      #ifdef ARDUINOJSON_VERSION_MAJOR >= 6
          DynamicJsonDocument jsonAutoControl(2048);
      #else
          DynamicJsonBuffer jsonBuffer;
          JsonObject& jsonAutoControl = jsonBuffer.createObject();
      #endif
      //using v6 only
      File file = SPIFFS.open("/config.json", "r");
      deserializeJson(jsonAutoControl, file);
      file.close();
      
      //////////LIGHT AUTO CONTROL/////////
      if( control == "light" ){
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
 
        jsonAutoControl["lightAuto"] = lightAuto;
        jsonAutoControl["lightOn"]   = lightOn;
        jsonAutoControl["lightHourOn"] = lightHourOn;
        jsonAutoControl["lightMinuteOn"] = lightMinuteOn;
        jsonAutoControl["lightHourOf"] = lightHourOf;
        jsonAutoControl["lightMinuteOff"] = lightMinuteOff;
        
        
      }
      //////////FAN LIGHT AUTO CONTROL/////////
      else if( control == "fan" ){
        
        fanAuto = 1;
        fanTempOn = (int)messageJSON["fanTempOn"];
        Serial.print("New Fan Auto ON Temperature: ");
        Serial.println(fanTempOn);
        jsonAutoControl["fanAuto"]   = fanAuto;
        jsonAutoControl["fanTempOn"] = fanTempOn;
        
      }
      //////////EXHAUST AUTO CONTROL/////////
      else if( control == "exhaust" ){
        
        exhaustAuto = 1;
        exhaustAirHumidityOn = (int)messageJSON["exhaustAirHumidityOn"];
        Serial.print("New Exhaust Auto ON Air Humidity: ");
        Serial.println(exhaustAirHumidityOn);
        jsonAutoControl["exhaustAuto"]          = exhaustAuto;
        jsonAutoControl["exhaustAirHumidityOn"] = exhaustAirHumidityOn;

      }

      else if( control == "water" ){

        waterAuto = 1;
        waterStartingHour = (int)messageJSON["waterStartingHour"];
        waterEveryXHour   = (int)messageJSON["waterEveryXHour"];
        waterEveryXDay    = (int)messageJSON["waterEveryXDay"];
        //lastWateredTime   = (int)messageJSON["lastWateredTime"]; // doesnt come from user...
        waterDurationSeconds   = (int)messageJSON["waterDurationSeconds"];
        Serial.print("New Water Auto. Water Starting Hour: ");
        Serial.println(waterStartingHour);
        Serial.print("Water Every X Day: ");
        Serial.println(waterEveryXDay);
        Serial.print("Duration in seconds: ");
        Serial.println(waterDurationSeconds);
        jsonAutoControl["waterAuto"]          = waterAuto;
        jsonAutoControl["waterStartingHour"] = waterStartingHour;
        //jsonAutoControl["waterEveryXHour"] = waterEveryXHour;
        jsonAutoControl["waterEveryXDay"] = waterEveryXDay;
        jsonAutoControl["lastWateredTime"] = lastWateredTime;
        jsonAutoControl["lastWateredHour"] = lastWateredHour;
        jsonAutoControl["waterDurationSeconds"] = waterDurationSeconds;
        
      }
      
      file = SPIFFS.open("/config.json", "w");
      serializeJson(jsonAutoControl, file);
      serializeJson(jsonAutoControl, Serial);
      file.close();

    }////////////////////////////////////////////// END AUTOMATIC CONTROL UPDATE ////////////////////////////////////////////////////

    //atoi doesnt work to convert from JSONVar to INT!!! becareful, in tthe other board it used to work....
    //int value = atoi(controlInfo[keys[1]]);

  }); //end of client callback

  /////////////////////////////////////////////END OF WS RECEIVE MESSAGES////////////////////////////////////////////////////

  /*Initialize temperature and humidity sensor.*/
  dht.begin();
}

void loop() {

  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;

  Serial.print("current date yyyy-mm-dd:");
  Serial.println( String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) );

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  Serial.print("Curr Time hh:mm:ss : ");
  Serial.print(currentHour);
  Serial.print(":");    
  Serial.print(currentMinute);
  Serial.print(":");
  Serial.println(currentSecond);
  
  String currentDate = String(monthDay) + "/" + String(currentMonth) + "/" + String(currentYear) + " " + String(currentHour) + ":" + String(currentMinute) + ":" + String(currentSecond);
  //String formattedTime = timeClient.getFormattedTime();
  
  /////////////////////////////////// PROCESS AUTOMATIC CONTROLS ////////////////////////////////////
  //////PROCESS AUTO LIGHT ////////
  if ( lightAuto == 1 )
  {

    Serial.println("inside lightAuto == 1. Print LightHourOn:LightMinutesOn");
    Serial.print(lightHourOn);
    Serial.print(":");
    Serial.println(lightMinuteOn);
    if (lightOn == 1) { //if lights are Off....
      if ( currentHour == lightHourOn && currentMinute == lightMinuteOn) {
        Serial.println("Insisde Light On == 0 and hour minutes are equal");
        lightOn = 0;
        pinMode(GPIO_LIGHT, OUTPUT);
        digitalWrite(GPIO_LIGHT, LOW); //LOW turns it ON, HIGH turns it OFF
        //save on board fs
        //EXPLODES....
        saveControlStatusFS(GPIO_LIGHT, lightOn);
      }
    }
    else if (lightOn == 0) { //if lights are ON....
      if ( currentHour == lightHourOf && currentMinute == lightMinuteOff) {        
        Serial.println("Insisde Light On == 1 and hour minutes are equal");
        lightOn = 1;
        pinMode(GPIO_LIGHT, OUTPUT);
        digitalWrite(GPIO_LIGHT, HIGH); //LOW turns it ON, HIGH turns it OFF
        //save on board fs
        //EXPLODES....
        saveControlStatusFS(GPIO_LIGHT, lightOn);
      }
    }
  }

  //////PROCESS AUTO FAN ////////
  if ( fanAuto == 1 )
  {

    Serial.println("inside fanAuto == 1. Fan On at Temperature:");
    Serial.println(fanTempOn);
    
    if (fanOn == 1) { //if fan is Off....
      if ( (int)dht.readTemperature() >= fanTempOn ) {
        Serial.println("Insisde Turn Fan On");
        fanOn = 0;
        pinMode(GPIO_FAN, OUTPUT);
        digitalWrite(GPIO_FAN, LOW); //LOW turns it ON, HIGH turns it OFF
        saveControlStatusFS(GPIO_FAN, fanOn);
      }
    }
    else if (fanOn == 0) { //if fan is ON....
      if ( (int)dht.readTemperature() < fanTempOn ) {        
        Serial.println("Inside Turn Fan Off");
        fanOn = 1;
        pinMode(GPIO_FAN, OUTPUT);
        digitalWrite(GPIO_FAN, HIGH); //LOW turns it ON, HIGH turns it OFF
        saveControlStatusFS(GPIO_FAN, fanOn);
      }
    }
  }

  //////PROCESS AUTO EXHAUST ////////
  if ( exhaustAuto == 1 )
  {

    Serial.println("inside exhaustAuto == 1. Exhaust On at air Humidity greater then or equal:");
    Serial.println(exhaustAirHumidityOn);
    
    if (exhaustOn == 1) { //if exhaust is Off....
      if ( (int)dht.readHumidity() >= exhaustAirHumidityOn ) {
        Serial.println("Insisde Turn Fan On");
        exhaustOn = 0;
        pinMode(GPIO_EXHAUST, OUTPUT);
        digitalWrite(GPIO_EXHAUST, LOW); //LOW turns it ON, HIGH turns it OFF
        saveControlStatusFS(GPIO_EXHAUST, exhaustOn);
      }
    }
    else if (exhaustOn == 0) { //if exhaust is ON....
      if ( (int)dht.readHumidity() < exhaustAirHumidityOn ) {        
        Serial.println("Inside Turn Fan Off");
        exhaustOn = 1;
        pinMode(GPIO_EXHAUST, OUTPUT);
        digitalWrite(GPIO_EXHAUST, HIGH); //LOW turns it ON, HIGH turns it OFF
        saveControlStatusFS(GPIO_EXHAUST, exhaustOn);
      }
    }
  }

  //////PROCESS AUTO WATER ////////
  if ( waterAuto == 1 )
  {
    Serial.println("inside WATER auto == 1.");
    Serial.print("Water on At Every X Day:");
    Serial.println(waterEveryXDay);
    Serial.print("Water starting hour:");
    Serial.println(waterStartingHour);
    Serial.print("Last watered time:");
    Serial.println(lastWateredTime);
    Serial.print("Water duration:");
    Serial.println(waterDurationSeconds);

    //SET day and time variables.
    
    if (waterOn == 1) { //if water is Off....

      // if auto water was never set up, just check if shold turn on.
      if ( lastWateredTime == 999 ){
        if( waterStartingHour == currentHour ){
        Serial.println("Insisde Turn WATER On for the first time");
        waterOn = 0;
        pinMode(GPIO_WATER, OUTPUT);
        digitalWrite(GPIO_WATER, LOW); //LOW turns it ON, HIGH turns it OFF
        //saveControlStatusFS(GPIO_WATER, waterOn);

        lastWateredTime = epochTime; //epoch time
        lastWateredHour = currentHour;
      
        //todo
        //save config to the fs.
        }
      }
      //if water is off and is not first time its runnin water auto, check if it should turn on by the difference of time last watered.
      else{
        unsigned long daysDiffLastWatering = ( epochTime - lastWateredTime ) / 86400;
        Serial.print("daysDiffLastWatering:");
        Serial.println(daysDiffLastWatering);
        int remainderDaysDiff = daysDiffLastWatering % waterEveryXDay;
        Serial.print("remainderDaysDiff: ");
        Serial.println(remainderDaysDiff);

        //if it is day of watering, check if it is time.
        if( remainderDaysDiff == 0 ){
          //if it is time to water, and last watered hour is not current hour, should turn on.
          if( waterEveryXHour == currentHour && lastWateredHour != currentHour ){
              Serial.println("Insisde Turn WATER On");
              waterOn = 0;
              pinMode(GPIO_WATER, OUTPUT);
              digitalWrite(GPIO_WATER, LOW); //LOW turns it ON, HIGH turns it OFF
              //saveControlStatusFS(GPIO_WATER, waterOn);
              
              lastWateredTime = epochTime; //epoch time
              lastWateredHour = currentHour;
          }
        }
      }
    }// end if water is off...

    //If current time is greater than last watered time plus the duration of watering in seconds....
    else if (waterOn == 0) { //if water is ON....
      
      Serial.println("Inside Turn WATER Off");
      unsigned long lastWateredTimePlusDuration = lastWateredTime + waterDurationSeconds;
      Serial.print("Last Watered Time:");
      Serial.println(lastWateredTime);
      Serial.print("waterDurationSeconds:");
      Serial.println(waterDurationSeconds);
      Serial.print("lastWateredTimePlusDuration:");
      Serial.println(lastWateredTimePlusDuration);

      if( epochTime > lastWateredTimePlusDuration ){
        waterOn = 1;
        pinMode(GPIO_WATER, OUTPUT);
        digitalWrite(GPIO_WATER, HIGH); //LOW turns it ON, HIGH turns it OFF
        //saveControlStatusFS(GPIO_WATER, waterOn);  
      }
      
    }
  }
  
  /////////////////////////////////// END OF PROCESS AUTOMATIC CONTROLS ////////////////////////////////////
  
  client.poll();
  
  // let the websockets client check for incoming messages
  if (client.available()) {
    //Serial.println("Print before client pool");
    //client.poll();
    //Serial.println("Client available after pool");

    ///////////////////////////////////////////// SEND SENSOR DATA /////////////////////////////////////////////
    String temperature = String(dht.readTemperature());
    String airHumidity    = String(dht.readHumidity());

    int    soilMoistureInt        = map(analogRead(soilPIN), aire, agua, 0, 100);
    String soilMoistureString     = String(soilMoistureInt);
    String JSONType               = "{\"type\":\"sensor\"";
    String currDateTime           = "\"dateTime\":\"" + currentDate + "\"";
    String sensorJSONTemperature  = "\"temperature\":\"" + temperature + "\"";
    String sensorJSONAirHumidity  = "\"airHumidity\":\"" + airHumidity + "\"";
    String sensorJSONSoilMoisture = "\"soilMoisture\":\"" + soilMoistureString + "\"}";

    String sensorJSONConcat = JSONType + "," + currDateTime + "," + sensorJSONTemperature + "," + sensorJSONAirHumidity + "," + sensorJSONSoilMoisture;
    
    Serial.print("*Serial:Sensor data ");
    Serial.println(sensorJSONConcat);

    client.send(sensorJSONConcat);
    ///////////////////////////////////////////// END OF SEND SENSOR DATA /////////////////////////////////////////////

    ///////////////////////////////////////////// SEND CONTROL DATA /////////////////////////////////////////////

    DynamicJsonDocument controlDoc(1024);
    controlDoc["type"] = "controlState";
    controlDoc["lightAuto"] = lightAuto; 
    controlDoc["lightOn"] = lightOn;
    controlDoc["fanAuto"] = fanAuto; 
    controlDoc["fanOn"] = fanOn;
    controlDoc["exhaustAuto"] = exhaustAuto; 
    controlDoc["exhaustOn"] = exhaustOn;
    controlDoc["waterAuto"] = waterAuto; 
    controlDoc["waterOn"] = waterOn;
    serializeJson(controlDoc, Serial);
    
    String controlString;
    serializeJson(controlDoc, controlString);
    client.send(controlString);

    ///////////////////////////////////////////// END OF SEND CONTROL DATA /////////////////////////////////////////////

  } 

  delay(1000);
}



///////HELPER FUNCTIONS/////

// ON = 0, OFF = 1
void saveControlStatusFS (int GPIO, int onOff){
     Serial.println("saving control manual to config file");
     Serial.print("GPIO:");
     Serial.println(GPIO);
     Serial.print("onOff:");
     Serial.println(onOff);
     
      #ifdef ARDUINOJSON_VERSION_MAJOR >= 6
          DynamicJsonDocument jsonControl(2048);
      #else
          DynamicJsonBuffer jsonBuffer;
          JsonObject& jsonControl = jsonBuffer.createObject();
      #endif
      
          //using v6 only
          File file = SPIFFS.open("/config.json", "r");
          deserializeJson(jsonControl, file);
          file.close();
          
          if( GPIO == GPIO_LIGHT ){
            jsonControl["lightOn"]   = onOff;
          }else if( GPIO == GPIO_FAN){
            jsonControl["fanOn"]   = onOff;
          }else if( GPIO == GPIO_EXHAUST){
            jsonControl["exhaustOn"]   = onOff;
          }else if( GPIO == GPIO_WATER){
            jsonControl["waterOn"]   = onOff;
          }
          
//          File configFile = SPIFFS.open("/config.json", "w");
//          if (!configFile) {
//            Serial.println("failed to open config file for writing auto control");
//          }
          file = SPIFFS.open("/config.json", "w");
          serializeJson(jsonControl, file);
          serializeJson(jsonControl, Serial);
          file.close();
          
//      #ifdef ARDUINOJSON_VERSION_MAJOR >= 6
//          serializeJson(jsonControl, Serial);
//          serializeJson(jsonControl, file);
//      #else
//          jsonControl.printTo(Serial);
//          jsonControl.printTo(file);
//      #endif
//          file.close();
          //end save
          //TODO
          //aparently, in order to update one field, needo to rewrite the whole config file
          //DynamicJsonDocument jsonControl(2048);
          //doc["key"] = "value";
}
