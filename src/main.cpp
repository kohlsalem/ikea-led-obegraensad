#include <Arduino.h>
#include <SPI.h>
#include <OneButton.h>

#ifdef ESP32
#include <WiFiManager.h>
#include "ConfigManager.h"
#endif
#ifdef ESP32
#include <ESPmDNS.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

#include "PluginManager.h"

#include "plugins/BreakoutPlugin.h"
#include "plugins/CirclePlugin.h"
#include "plugins/DrawPlugin.h"
#include "plugins/FireworkPlugin.h"
#include "plugins/GameOfLifePlugin.h"
#include "plugins/LinesPlugin.h"
#include "plugins/RainPlugin.h"
#include "plugins/SnakePlugin.h"
#include "plugins/StarsPlugin.h"
#include "plugins/PongClockPlugin.h"

#ifdef ENABLE_SERVER
#include "plugins/AnimationPlugin.h"
#include "plugins/BigClockPlugin.h"
#include "plugins/ClockPlugin.h"
#include "plugins/WeatherPlugin.h"
#include "plugins/AnimationPlugin.h"
#include "plugins/TickingClockPlugin.h"
#endif

#include "asyncwebserver.h"
#include "ota.h"
#include "screen.h"
#include "secrets.h"
#include "websocket.h"
#include "messages.h"

unsigned long previousMillis = 0;
unsigned long interval = 30000;

PluginManager pluginManager;
SYSTEM_STATUS currentStatus = NONE;

#ifdef ESP32
WiFiManager wifiManager;
//flag for saving data for custom wifimanager parameters
bool shouldSaveConfig = false;
#endif

unsigned long lastConnectionAttempt = 0;
const unsigned long connectionInterval = 10000;

/**
 * Initialize a new OneButton instance for a button
 * connected to digital pin 4 and GND, which is active low
 * and uses the internal pull-up resistor.
 */

OneButton btn = OneButton(
  PIN_BUTTON,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);


#ifdef ESP32
char timeZone[40]; // Zeitzone als Zeichenkette speichern

 //callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void connectToWiFi()
{

  // if a WiFi setup AP was started, reboot is required to clear routes
  bool wifiWebServerStarted = false;
  wifiManager.setWebServerCallback(
      [&wifiWebServerStarted]()
      { wifiWebServerStarted = true; });

  wifiManager.setHostname(WIFI_HOSTNAME);
  
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // InputParameter fot Timezone
  WiFiManagerParameter customTimeZone("pTimeZone","Time Zone","CET-1CEST,M3.5.0,M10.5.0/3",39);
  wifiManager.addParameter(&customTimeZone);



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
  btn.attachLongPressStop([](){
    // if a long press is more then 5 seconds
    if(btn.getPressedMs()>5000){
      WiFiManagerParameter customTimeZone("TimeZone", "Enter Timezone according to the list: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.json", timeZone, 40);

      wifiManager.addParameter(&customTimeZone);

      Screen.scrollText("Starting Configuration Portal");
      server.end(); // stop actual webserver
      wifiManager.startConfigPortal(WIFI_HOSTNAME); // trigger config portal
      Screen.scrollText("Config Finished");
      ESP.restart();
    }
  });
}
#endif

#ifdef ESP8266
void connectToWiFi()
{
  Serial.println("Connecting to Wi-Fi...");

  // Delete old config
  WiFi.disconnect(true);

#if defined(IP_ADDRESS) && defined(GWY) && defined(SUBNET) && defined(DNS1) && \
    defined(DNS2)
  auto ip = IPAddress();
  ip.fromString(IP_ADDRESS);

  auto gwy = IPAddress();
  gwy.fromString(GWY);

  auto subnet = IPAddress();
  subnet.fromString(SUBNET);

  auto dns1 = IPAddress();
  dns1.fromString(DNS1);

  auto dns2 = IPAddress();
  dns2.fromString(DNS2);

  WiFi.config(ip, gwy, subnet, dns1, dns2);
#endif

  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  // Check connection result
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nFailed to connect to Wi-Fi. Please check credentials.");
  }

  lastConnectionAttempt = millis();
}
#endif

void setup()
{
  Serial.begin(115200);

  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

// server
#ifdef ENABLE_SERVER
  connectToWiFi();

  // set time server
  configTzTime(TZ_INFO, NTP_SERVER);

  initOTA(server);
  initWebsocketServer(server);
  initWebServer();
#endif

  Screen.setup();

  pluginManager.addPlugin(new DrawPlugin());
  pluginManager.addPlugin(new BreakoutPlugin());
  pluginManager.addPlugin(new SnakePlugin());
  pluginManager.addPlugin(new GameOfLifePlugin());
  pluginManager.addPlugin(new StarsPlugin());
  pluginManager.addPlugin(new LinesPlugin());
  pluginManager.addPlugin(new CirclePlugin());
  pluginManager.addPlugin(new RainPlugin());
  pluginManager.addPlugin(new FireworkPlugin());
  pluginManager.addPlugin(new PongClockPlugin());

#ifdef ENABLE_SERVER
  pluginManager.addPlugin(new BigClockPlugin());
  pluginManager.addPlugin(new ClockPlugin());
  pluginManager.addPlugin(new WeatherPlugin());
  pluginManager.addPlugin(new AnimationPlugin());
  pluginManager.addPlugin(new TickingClockPlugin());
#endif

  pluginManager.init();

  // Attach method to switch plugin at button click
  btn.attachClick([](){
    if (currentStatus != LOADING)
    {
      pluginManager.activateNextPlugin();
      currentStatus = NONE;
    }
  });
}

void loop()
{

  btn.tick();
  Messages.scrollMessageEveryMinute();

  pluginManager.runActivePlugin();

  if (WiFi.status() != WL_CONNECTED && millis() - lastConnectionAttempt > connectionInterval)
  {
    Serial.println("Lost connection to Wi-Fi. Reconnecting...");
    connectToWiFi();
  }

#ifdef ENABLE_SERVER
  cleanUpClients();
#endif
  delay(1);
}