#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <ArduinoJson.h>
#include <WiFiManager.h>

class ConfigManager_ {
private:
    const char *filename;
    StaticJsonDocument<200> config;
    WiFiManager wifiManager;
public:
    ConfigManager_(const char *filename);

    void loadConfig();
    void saveConfig();

    const char *getTimezone();
    void setTimezone(const char *timezone);

    const char *getStartTime();
    void setStartTime(const char *starttime);

    const char *getEndTime();
    void setEndTime(const char *endtime);

    void addParameter(WiFiManagerParameter &parameter);
    String getWiFiManagerResult();
};
extern ConfigManager_ &ConfigManager;
#endif
