#include <ArduinoJson.h> // to deal with JSON FS stuff
#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager


//Wifi Manager configuration
//flag for saving data. (Not sure this is necessary)
bool shouldSaveConfig = false;
//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

/* Board token that will be populated when user signs in to Wifi */
char tokenChar[40];
String token = "initToken";
String connectionURL = "InitURL";


void setup() {
  
Serial.begin(115200);

  /////////////////////////////READ CONFIGURATION FROM FS JSON///////////////////////////////
  Serial.println("mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);

        DynamicJsonDocument doc(1024); //old way DynamicJsonBuffer
        //JsonObject& json = 
        DeserializationError error = deserializeJson(doc, buf.get()); // old way //JsonObject& json = jsonBuffer.parseObject(buf.get());
        if(error){
          Serial.print("desirializeJson failed with code: ");
          Serial.println(error.c_str());
          return;
        }
        else{
            Serial.println("\nparsed json");
            strcpy(tokenChar, doc["tokenChar"]);
//          strcpy(mqtt_server, json["mqtt_server"]);
            Serial.print("token char:");
            Serial.println(tokenChar);
          }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  /////////////////////////////END OF READ CONFIGURATION FROM FS JSON///////////////////////////////

  /////////////////////////////WIFI MANAGER SET UP AND CONNECT///////////////////////////////
  WiFiManagerParameter tokenWifiParameter("tokenChar", "Station Token", tokenChar, 40);

  WiFiManager wifiManager;
  
  //set config save notify callback (is there a case where I dont want it to be saved? not sure this is necessary..)
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  //wifiManager.autoConnect("Cannastation");

  //add all your parameters here
  wifiManager.addParameter(&tokenWifiParameter);
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
    ESP.reset();
    delay(5000);
  }
  Serial.println("connected to wifi...yeey :)");
  
  /////////////////////////////END OF WIFI MANAGER SET UP AND CONNECT///////////////////////////////

  /////////////////////////////SAVING WIFI MANAGER PARATEMERS TO THE BOARD FS///////////////////////////////
  //read updated parameters
  strcpy(tokenChar, tokenWifiParameter.getValue());
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");

    DynamicJsonDocument doc(1024); //old way DynamicJsonBuffer

    doc["tokenChar"] = tokenChar;
    //    json["mqtt_server"] = mqtt_server;
//    JsonArray data = doc.createNestedArray("data");
//    data.add(48.12123)
//    data.add(2.012312)
    serializeJson(doc, Serial);
    Serial.println();
    serializeJsonPretty(doc, Serial);
    
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    
//    json.printTo(Serial);
//    json.printTo(configFile);
    
    configFile.close();
  }
  /////////////////////////////END OF SAVING WIFI MANAGER PARATEMERS TO THE BOARD FS///////////////////////////////

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

}

void loop() {

    // Check WiFi connection status
  if (WiFi.isConnected()) {
  // put your main code here, to run repeatedly:
      Serial.println("Connected, inside loop");

  }
  else {
      Serial.println("WiFi Disconnected");
    }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  delay(3000);
  
}
