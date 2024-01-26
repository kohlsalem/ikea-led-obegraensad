#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <ArduinoJson.h>
#include <WiFiManager.h>

#include "constants.h"
#include "secrets.h"
#include <FS.h>

#include <SPIFFS.h>
#include <ESPmDNS.h>
#include "screen.h"
#include "asyncwebserver.h"

class ConfigManager_ {
private:
    const char *filename="settings.cfg";
    StaticJsonDocument<200> config;
    WiFiManager wifiManager;
    //flag for saving data for custom wifimanager parameters
    bool shouldSaveConfig = false;
    char timeZone[40]; // Zeitzone als Zeichenkette speichern
    unsigned long lastConnectionAttempt = 0;
    const unsigned long connectionInterval = 10000;
public:
    static ConfigManager_ &getInstance();

    void loadConfig();
    void saveConfig();

    const char *getTimezone();
    void setTimezone(const char *timezone);

    const char *getStartTime();
    void setStartTime(const char *starttime);

    const char *getEndTime();
    void setEndTime(const char *endtime);

    void saveConfigCallback () ;
    void longPressReleaseCalllback();
    void connectToWiFi();
};
extern ConfigManager_ &ConfigManager;
#endif
