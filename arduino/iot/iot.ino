/** -------------------------------------
   Vfc PWA Iot by ValterFC
   Based on ESP8622 Wemos D1 Mini
   https://gitlab.com/ValterFC/Vfc_AquaControl

   PWA Page
   https://pwaiot.github.io

   Source code
   https://github.com/pwaiot/pwaiot.github.io

   This code is released to the "Attribution-NonCommercial 3.0 Unported (CC BY-NC 3.0)"
   http://creativecommons.org/licenses/by-nc/3.0/

------------------------------------- */

//If you use double quotes for your #include rather than angle brackets it will look locally

//TaskScheduler defintions
//#define _TASK_SLEEP_ON_IDLE_RUN // Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were invoked during the pass 
//#define _TASK_STATUS_REQUEST  // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
#define _TASK_TIMECRITICAL // Support for layered scheduling priority

//global includes
#include "config.h"
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#ifdef USE_ESP32
  //#include <WiFi.h>
#endif
#ifdef USE_ESP8266
  //#include <ESP8266WiFi.h>
#endif

#include <TaskScheduler.h>

//local includes
#include "mqtt_com.h"
#include "web_time.h"

int ledVal = 0; // variable to store the read value

// Prototypes
void wifiSetup();

// Callback methods prototypes
void ledCallback();
void initCallback();

//Tasks runner
Scheduler runner;

//Tasks
//Task( unsigned long aInterval, long aIterations, TaskCallback aCallback, Scheduler* aScheduler, bool aEnable, TaskOnEnable aOnEnable, TaskOnDisable aOnDisable )
Task taskLed       (  500, TASK_FOREVER, &ledCallback              , &runner, true);
Task taskMQTTRec   (  100, TASK_FOREVER, &mqttcom_handleClient     , &runner, false); //fast receive
Task taskWebTime   ( 1000, TASK_ONCE   , &webtime_handle           , &runner, false); //NTP web time

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  pinMode(LED_PIN, OUTPUT); //set LED_PIN as output

  //init serial communication
  USESERIAL.begin(SERIALSPEED);
  USESERIAL.println("booting");

  runner.startNow(); // set point-in-time for scheduling start
  USESERIAL.println("Initialized task scheduler");

  //init wifi
  wifiSetup();

  //MQTT Mosquitto Setup
  mqttcom_setup();

  webtime_setup();
  
  taskMQTTRec.enable();
  taskWebTime.enable();
  USESERIAL.println("all tasks initialized successfully");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
  runner.execute(); //run tasks
  mqttcom_handleClient(); //debug
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ledCallback() {
  ledVal = digitalRead(LED_PIN); //LED_OFF / LED_ON
  digitalWrite(LED_PIN, !ledVal); //builtin LED ON (inverse logic sign for PIN 2)
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Wifi
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void wifiSetup() {
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  
  //Connect
  wifiManager.autoConnect(MY_HOSTNAME);
  
  //Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
