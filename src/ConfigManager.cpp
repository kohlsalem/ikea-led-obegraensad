#include "ConfigManager.h"


void ConfigManager_::loadConfig() {
  // Open the configuration file in read mode
  File configFile = LittleFS.open(filename, "r");

  // Check if the file is successfully opened
  if (configFile) {
    // Parse the JSON configuration from the file
    DeserializationError error = deserializeJson(config, configFile);

    // Close the configuration file
    configFile.close();

    // Check for parsing errors
    if (error) {
      // Print an error message if there was an issue parsing the configuration
      Serial.print("Failed to parse configuration file: ");
      Serial.println(error.c_str());
    } else {
      // Configuration loaded successfully, print a success message
      Serial.println("Configuration loaded successfully");

      // Retrieve individual configuration parameters and update variables
      strncpy(nightstartbuf, config["NightStartTime"], sizeof(nightstartbuf) - 1);
      strncpy(nightendbuf, config["NightEndTime"], sizeof(nightendbuf) - 1);
      strncpy(timezonebuf, config["Timezone"], sizeof(timezonebuf) - 1);

      // Ensure null-termination of the strings
      nightstartbuf[sizeof(nightstartbuf) - 1] = '\0';
      nightendbuf[sizeof(nightendbuf) - 1] = '\0';
      timezonebuf[sizeof(timezonebuf) - 1] = '\0';
    }
  } else {
    // Print a message if the file couldn't be opened
    Serial.println("Error opening configuration file for reading. Using default values.");

    // Set default values or initialize variables
    strncpy(nightstartbuf, "22:30", sizeof(nightstartbuf) - 1);
    strncpy(nightendbuf, "6:30", sizeof(nightendbuf) - 1);
    strncpy(timezonebuf, TZ_INFO, sizeof(timezonebuf) - 1);

    // Ensure null-termination of the strings
    nightstartbuf[sizeof(nightstartbuf) - 1] = '\0';
    nightendbuf[sizeof(nightendbuf) - 1] = '\0';
    timezonebuf[sizeof(timezonebuf) - 1] = '\0';
  }
}

void ConfigManager_::saveConfig() {
  // Check if there are configuration changes to be saved
  if (shouldSaveConfig) {
    // Open the configuration file in write mode
    File configFile = LittleFS.open(filename, "w");
    
    // Check if the file is successfully opened
    if (configFile) {
      // Update configuration parameters with the latest values
      config["NightStartTime"] = nightstartbuf;
      config["NightEndTime"] = nightendbuf;
      config["Timezone"] = timezonebuf;

      // Serialize the JSON configuration to the file
      serializeJson(config, configFile);

      // Close the configuration file
      configFile.close();

      // Print a message indicating that the configuration has been saved
      Serial.println("Configuration saved successfully");
    } else {
      // Print an error message if the file couldn't be opened
      Serial.println("Error opening configuration file for writing");
    }
  } else {
    // Print a message if there are no configuration changes to be saved
    Serial.println("No configuration changes to save");
  }
}

/*
void ConfigManager_::loadConfig() {
  // Open the configuration file in read mode
  File configFile = SPIFFS.open(filename, "r");

  // Check if the file is successfully opened
  if (configFile) {
    // Parse the JSON configuration from the file
    DeserializationError error = deserializeJson(config, configFile);

    // Close the configuration file
    configFile.close();

    // Check for parsing errors
    if (error) {
      // Print an error message if there was an issue parsing the configuration
      Serial.print("Failed to parse configuration file: ");
      Serial.println(error.c_str());
    } else {
      // Configuration loaded successfully, print a success message
      Serial.println("Configuration loaded successfully");

      // Retrieve individual configuration parameters and update variables
      strncpy(nightstartbuf, config["NightStartTime"], sizeof(nightstartbuf) - 1);
      strncpy(nightendbuf, config["NightEndTime"], sizeof(nightendbuf) - 1);
      strncpy(timezonebuf, config["Timezone"], sizeof(timezonebuf) - 1);

      // Ensure null-termination of the strings
      nightstartbuf[sizeof(nightstartbuf) - 1] = '\0';
      nightendbuf[sizeof(nightendbuf) - 1] = '\0';
      timezonebuf[sizeof(timezonebuf) - 1] = '\0';
    }
  } else {
    // Print a message if the file couldn't be opened
    Serial.println("Error opening configuration file for reading. Using default values.");

    // Set default values or initialize variables
    strncpy(nightstartbuf, "22:30", sizeof(nightstartbuf) - 1);
    strncpy(nightendbuf, "6:30", sizeof(nightendbuf) - 1);
    strncpy(timezonebuf, TZ_INFO, sizeof(timezonebuf) - 1);

    // Ensure null-termination of the strings
    nightstartbuf[sizeof(nightstartbuf) - 1] = '\0';
    nightendbuf[sizeof(nightendbuf) - 1] = '\0';
    timezonebuf[sizeof(timezonebuf) - 1] = '\0';
  }
}

void ConfigManager_::saveConfig() {
  // Check if there are configuration changes to be saved
  if (shouldSaveConfig) {
    // Open the configuration file in write mode
    File configFile = SPIFFS.open(filename, "w");
    
    // Check if the file is successfully opened
    if (configFile) {
      // Update configuration parameters with the latest values
      config["NightStartTime"] = nightstartbuf;
      config["NightEndTime"] = nightendbuf;
      config["Timezone"] = timezonebuf;

      // Serialize the JSON configuration to the file
      serializeJson(config, configFile);

      // Close the configuration file
      configFile.close();

      // Print a message indicating that the configuration has been saved
      Serial.println("Configuration saved successfully");
    } else {
      // Print an error message if the file couldn't be opened
      Serial.println("Error opening configuration file for writing");
    }
  } else {
    // Print a message if there are no configuration changes to be saved
    Serial.println("No configuration changes to save");
  }
}*/

// callback notifying us of the need to save config
void ConfigManager_::saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

const char *ConfigManager_::getTimezone()
{
  return config["timezone"] | "Default_Timezone";
}

void ConfigManager_::setTimezone(const char *timezone)
{
  config["timezone"] = timezone;
}

const char *ConfigManager_::getStartTime()
{
  return config["starttime"] | "Default_Starttime";
}

void ConfigManager_::setStartTime(const char *starttime)
{
  config["starttime"] = starttime;
}

const char *ConfigManager_::getEndTime()
{
  return config["endtime"] | "Default_Endtime";
}

void ConfigManager_::setEndTime(const char *endtime)
{
  config["endtime"] = endtime;
}

void ConfigManager_::setupWifiManagerConfiguration(WiFiManager &wifiManager)
{
  static WiFiManagerParameter nightstart_param("starttime", "NightMode stars at [hh:mm]", nightstartbuf, 5);
  static WiFiManagerParameter nightend_paam("endtime", "Night Mode ends at [hh:mm]", nightendbuf, 5);
  static WiFiManagerParameter timezone_param("timezone", "Posix Timezone", timezonebuf, 40);
  // add all your parameters here
  wifiManager.addParameter(&nightstart_param);
  wifiManager.addParameter(&nightend_paam);
  wifiManager.addParameter(&timezone_param);

  wifiManager.setDarkMode(true); // idk, dark mot fits just better to that black display
  shouldSaveConfig = false;
  wifiManager.setSaveConfigCallback([]()
                                    { Serial.println("SaveConfigCallback");ConfigManager.saveConfigCallback(); });
}

void ConfigManager_::startConfigPortal()
{

    Screen.scrollText("Starting Configuration Portal");
    server.end(); // stop actual webserver

    WiFiManager wifiManager;

    setupWifiManagerConfiguration(wifiManager);

    Serial.print("startConfigPortal");
    wifiManager.startConfigPortal(WIFI_MANAGER_SSID); // trigger config portal
    
    shouldSaveConfig = true;//aparently the callback does not work as expected
    saveConfig();
    
    Serial.print("endConfigPortal");
    Serial.print(nightstartbuf);
    Serial.print(nightendbuf);
    Serial.print(timezonebuf);

    Screen.scrollText("Config Finished");
    ESP.restart();
  
}

void ConfigManager_::connectToWiFi()
{

  WiFiManager wifiManager;
  // if a WiFi setup AP was started, reboot is required to clear routes
  bool wifiWebServerStarted = false;

  loadConfig();

  wifiManager.setWebServerCallback(
      [&wifiWebServerStarted]()
      { wifiWebServerStarted = true; });

  setupWifiManagerConfiguration(wifiManager);

  wifiManager.setHostname(WIFI_HOSTNAME);
  /*
    //set config save notify callback
    wifiManager.setSaveConfigCallback([](){ConfigManager.saveConfigCallback();});

    // InputParameter for Timezone
    WiFiManagerParameter customTimeZone("pTimeZone","Time Zone","CET-1CEST,M3.5.0,M10.5.0/3",39);
    wifiManager.addParameter(&customTimeZone);
  */

  wifiManager.autoConnect(WIFI_MANAGER_SSID);

  this->saveConfig();

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
}

ConfigManager_ &ConfigManager_::getInstance()
{
  static ConfigManager_ instance;
  return instance;
}

ConfigManager_ &ConfigManager = ConfigManager_::getInstance();