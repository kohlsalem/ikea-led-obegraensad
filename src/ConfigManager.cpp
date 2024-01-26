#include "ConfigManager.h"




void ConfigManager_::loadConfig() {
    File configFile = SPIFFS.open(filename, "r");
    if (configFile) {
        deserializeJson(config, configFile);
        configFile.close();
    }
}
 //callback notifying us of the need to save config
void ConfigManager_::saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void ConfigManager_::saveConfig() {
    File configFile = SPIFFS.open(filename, "w");
    if (configFile) {
        serializeJson(config, configFile);
        configFile.close();
    }
}

const char *ConfigManager_::getTimezone() {
    return config["timezone"] | "Default_Timezone";
}

void ConfigManager_::setTimezone(const char *timezone) {
    config["timezone"] = timezone;
}

const char *ConfigManager_::getStartTime() {
    return config["starttime"] | "Default_Starttime";
}

void ConfigManager_::setStartTime(const char *starttime) {
    config["starttime"] = starttime;
}

const char *ConfigManager_::getEndTime() {
    return config["endtime"] | "Default_Endtime";
}

void ConfigManager_::setEndTime(const char *endtime) {
    config["endtime"] = endtime;
}
char mqtt_server[40];
char mqtt_port[6] = "8080";
char api_token[34] = "YOUR_APITOKEN";

 void ConfigManager_::longPressReleaseCalllback(){
    // if a long press is more then 5 seconds
    if(btn.getPressedMs()>5000){
      Screen.scrollText("Starting Configuration Portal");
      server.end(); // stop actual webserver

      WiFiManager wm;
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
  WiFiManagerParameter custom_api_token("apikey", "API token", api_token, 34);
  //add all your parameters here
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_api_token);

      wm.startConfigPortal(WIFI_MANAGER_SSID); // trigger config portal
      
      
      
      Screen.scrollText("Config Finished");
      ESP.restart();
    }
 }

 void ConfigManager_::connectToWiFi()
{

  // if a WiFi setup AP was started, reboot is required to clear routes
  bool wifiWebServerStarted = false;
  wifiManager.setWebServerCallback(
      [&wifiWebServerStarted]()
      { wifiWebServerStarted = true; });

  wifiManager.setHostname(WIFI_HOSTNAME);
/*  
  //set config save notify callback
  wifiManager.setSaveConfigCallback([](){ConfigManager.saveConfigCallback();});

  // InputParameter for Timezone
  WiFiManagerParameter customTimeZone("pTimeZone","Time Zone","CET-1CEST,M3.5.0,M10.5.0/3",39);
  wifiManager.addParameter(&customTimeZone);
*/
 // id/name, placeholder/prompt, default, length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
  WiFiManagerParameter custom_api_token("apikey", "API token", api_token, 34);
  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_api_token);


  wifiManager.setDarkMode(true); //idk, dark mot fits just better to that black display

  wifiManager.autoConnect(WIFI_MANAGER_SSID);

  if (MDNS.begin(WIFI_HOSTNAME))
  {
    MDNS.addService("http", "tcp", 80);
    MDNS.setInstanceName(WIFI_HOSTNAME);
  }
  else
  {
    Serial.println("Could not start mDNS!");
  }

  if (wifiWebServerStarted)
  {
    // Reboot required, otherwise wifiManager server interferes with our server
    Serial.println("Done running WiFi Manager webserver - rebooting");
    ESP.restart();
  }

  lastConnectionAttempt = millis();

  // Attach Action to button longpress to start config portal
  btn.attachLongPressStop([](){ConfigManager.longPressReleaseCalllback();});

}

ConfigManager_ &ConfigManager_::getInstance()
{
  static ConfigManager_ instance;
  return instance;
}

ConfigManager_ &ConfigManager = ConfigManager_::getInstance();