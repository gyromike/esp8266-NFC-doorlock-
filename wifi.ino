

//callback notifying us of the need to save config
void saveConfigCallback () {
  shouldSaveConfig = true;
}

void startmywifi() {
  if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      json.printTo(Serial);
      if (json.success()) {
        // json will parse the file for the variable name and load it into the supplied variable
        strcpy(module_name, json["module"]);   //get the module name
        strcpy(reader_token, json["Reader"]);   //get the blynk token
        strcpy(lock_token, json["Lock"]);       //get the blynk token
      }
      configFile.close();
    }
  }
  //Local intialization. Once its done, there is no need to keep it around
  WiFiManager wifiManager;
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // The extra parameters to be configured (can be either global or just here)
  // After connecting, parameter.getValue() will get you the configured value
  // name, prompt, default, length
  WiFiManagerParameter custom_module_name("module", "module name", module_name, MODULENAMESIZE);
  WiFiManagerParameter custom_blynk_token_1("Reader", "Reader token", reader_token, BLYNKSIZE);
  WiFiManagerParameter custom_blynk_token_2("Lock", "Lock token", lock_token, BLYNKSIZE);

  //  <============== add all your parameters here
  wifiManager.addParameter(&custom_module_name);
  wifiManager.addParameter(&custom_blynk_token_1);
  wifiManager.addParameter(&custom_blynk_token_2);

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(120);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    ESP.reset();    //reset and try again
  }

  //get the custom parameters here
  strcpy(module_name, custom_module_name.getValue());
  strcpy(reader_token, custom_blynk_token_1.getValue());
  strcpy(lock_token, custom_blynk_token_2.getValue());

  //check the flag to see if we save the custom parameters to FS
  if (shouldSaveConfig) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    //set the custom parameters here
    json["module"] = module_name;
    json["Reader"] = reader_token;
    json["Lock"] = lock_token;


    File configFile = SPIFFS.open("/config.json", "w");
    if (configFile) {
      json.printTo(configFile);   //write the custom parameters here
      configFile.close();
    } else Serial.println("failed to open config file for writing");
    //end save
  }
}
