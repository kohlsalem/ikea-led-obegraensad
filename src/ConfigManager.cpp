#include "ConfigManager.h"
#include <FS.h>
#ifdef ESP32
  #include <SPIFFS.h>
#endif
ConfigManager_::ConfigManager_(const char *filename) {
    this->filename = filename;
}

void ConfigManager_::loadConfig() {
    File configFile = SPIFFS.open(filename, "r");
    if (configFile) {
        deserializeJson(config, configFile);
        configFile.close();
    }
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

void ConfigManager_::addParameter(WiFiManagerParameter &parameter) {
    wifiManager.addParameter(&parameter);
}

String ConfigManager_::getWiFiManagerResult() {
    String result;
    result.reserve(256);  // Adjust the size based on your needs
    result += "WiFi SSID: " + WiFi.SSID() + "\n";
    result += "WiFi Password: " + WiFi.psk() + "\n";
    result += "IP Address: " + WiFi.localIP().toString() + "\n";
    // Add more information as needed
    return result;
}

ConfigManager_ configManager("/config.json");