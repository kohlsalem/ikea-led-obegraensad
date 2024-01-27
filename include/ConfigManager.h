#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <ArduinoJson.h>
#include <WiFiManager.h>

#include "constants.h"
#include "secrets.h"
#include <FS.h>

//#include <SPIFFS.h>
#include <LittleFS.h>

#include <ESPmDNS.h>
#include "screen.h"
#include "asyncwebserver.h"

class ConfigManager_ {
private:
   //String Buffers for additional parameters
    char nightstartbuf[6] = "22:30";
    char nightendbuf[6] = "6:30";
    char timezonebuf[41] = TZ_INFO;
    int nightstart;
    int nightend;


    const char *filename="settings.cfg";
    StaticJsonDocument<200> config;

    //flag for saving data for custom wifimanager parameters
    bool shouldSaveConfig = false;
    char timeZone[40]; // Zeitzone als Zeichenkette speichern
    unsigned long lastConnectionAttempt = 0;
    const unsigned long connectionInterval = 10000;
    void setupWifiManagerConfiguration(WiFiManager &wifiManager);
    ConfigManager_();
    int convertToMinutes(const char* timeStr);
public:
    static ConfigManager_ &getInstance();

    void loadConfig();
    void saveConfig();

    const char *getTimezone();

    int getNightStart();
    int getNightEnd();

    void saveConfigCallback () ;
    void startConfigPortal();
    void connectToWiFi();
};
extern ConfigManager_ &ConfigManager;
#endif
