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
#include "config.h" //copy "config.example.h" to "config.h" and edit it

#ifdef USE_ESP32
  #include <WiFi.h>
#endif
#ifdef USE_ESP8266
  #include <ESP8266WiFi.h>
#endif

#include <TaskScheduler.h>

//local includes
#include "mqtt_com.h"
#include "web_time.h"

int ledVal = 0; // variable to store the read value

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
  initWiFi();

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
  reconnectWiFi();
  mqttcom_handleClient(); //debug
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//IPAddress myIP; // = WiFi.softAPIP();

void wifiDoConnection() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //WiFi.disconnect();
  //WiFi.softAP(ssid, password);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initWiFi() {
  //hostname
  #ifdef USE_ESP32
    WiFi.setHostname(MY_HOSTNAME);
  #elif USE_ESP32 //ESP8266
    WiFi.hostname(MY_HOSTNAME);
  #endif  

  wifiDoConnection();

  taskLed.setInterval(25); //fast blink
  taskLed.enable();
  runner.execute(); //run tasks
  USESERIAL.print("Connecting");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //USESERIAL.println("Connection Failed! Rebooting...");
    USESERIAL.print(".");
    runner.execute(); //run tasks
    delay(100);
    //ESP.restart();
  }
  USESERIAL.println();
  taskLed.setInterval(500); //fast blink
  taskLed.enable();

  //myIP = WiFi.softAPIP();
  
  #ifdef USE_ESP32
    USESERIAL.printf("New hostname: %s\n", WiFi.getHostname());
  #elif USE_ESP32 //ESP8266
    USESERIAL.printf("New hostname: %s\n", WiFi.hostname().c_str());
  #endif  
  USESERIAL.println();
  USESERIAL.print("IP address: ");
  //USESERIAL.println(myIP);
  USESERIAL.println(WiFi.localIP());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void reconnectWiFi() {
  //if you are already connected to the WI-FI network, nothing is done.
  //Otherwise connection attempts are made
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  taskLed.setInterval(25); //fast blink
  taskLed.enable();
       
  wifiDoConnection();
   
  USESERIAL.print("Waiting to connect");
  
  while (WiFi.status() != WL_CONNECTED) //Wait for connection
  {
      delay(100);
      USESERIAL.print(".");
  }
  taskLed.setInterval(500); //slow blink
  taskLed.enable();

  //myIP = WiFi.softAPIP();
 
  USESERIAL.println();
  USESERIAL.print("IP address: ");
  //USESERIAL.println(myIP);
  USESERIAL.println(WiFi.localIP());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ledCallback() {
  ledVal = digitalRead(LED_PIN); //LED_OFF / LED_ON
  digitalWrite(LED_PIN, !ledVal); //builtin LED ON (inverse logic sign for PIN 2)
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
