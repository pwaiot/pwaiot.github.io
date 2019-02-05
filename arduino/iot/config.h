#define BOARD_NODEMCU
//#define BOARD_D1_MINI

//#define USE_ESP32 //to use ESP32   - instead of ESP8266
#define USE_ESP8266 //to use ESP8266 - instead of ESP32

#define USESERIAL Serial //Serial1
#define SERIALSPEED 74880  //74880 //for me, it´s better speed; you can change this to 115200

#include "sensors_board.h"

//UTC
const int16_t myUTC = -2; //timezone UTC -2:00 Brazil

//wifi configuration
//const char* ssid     = "ssid_to_connect"
//const char* password = "ssid_password"

//hostname
#define MY_HOSTNAME "vfcpwaiot"

//------------------
//mqtt configuration
//------------------
//mqtt client
const char* BROKER_MQTT = "iot.eclipse.org";
const int   BROKER_PORT = 1883;
const char* BROKER_USER = "";
const char* BROKER_PWD  = "";

//define mqtt id and topics for publishing and subscribe
#define SUBSCRIBE_COMMANDS "Vfc/Accellog/Lab/Developer/PWAIoT/Test01/Commands"  //listening MQTT topic
#define PUBLISH_DASHBOARD  "Vfc/Accellog/Lab/Developer/PWAIoT/Test01/Dashboar" //topic MQTT to send information to Broker

                                                       //IMPORTANT: We strongly recommend changing the names
                                                       //           these topics. Otherwise, there are large
                                                       //           chances of you controlling and monitoring NodeMCU
                                                       //           of another person.

#define ID_MQTT  "VfcEspPwaIoT"   //id mqtt (for session identification)
                                  //IMPORTANT: This must be unique in the broker (in other words,
                                  //           if an MQTT client attempts to enter the same
                                  //           id of another already connected to the broker, the broker
                                  //           will close the connection of one of them).
