#include <config.h> //copy "config.example.h" to "config.h" and edit it
#include <Time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <PubSubClient.h> // Import the PubSubClient Library

#ifdef USE_ESP32
  #include <WiFi.h>
  #include <ESPmDNS.h>
#endif
#ifdef USE_ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
#endif  

typedef void (*mqttclient_datacallback) (String topic, String message);

//Prototypes
void mqttcom_setup();
void mqttcom_handleClient();

//private
void mqttclient_setup();
void mqttclient_handle();
void mqttclient_setCallback(mqttclient_datacallback cb);
void initMQTT();
void mqttcom_callback(String topic, String msg);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttcom_setup() {
    initMQTT();
    sensors_Setup();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initMQTT() {
  mqttclient_setup();
  mqttclient_setCallback(mqttcom_callback);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttcom_callback(String topic, String msg) {
    USESERIAL.print("MQTT topic received: ");
    USESERIAL.println(topic);
    
    //if(strcmp(topic, SUBSCRIBE_COMMANDS) == 0) {
    if (topic == SUBSCRIBE_COMMANDS) {
        USESERIAL.print("MQTT msg received: ");
        USESERIAL.println(msg);

        // -----------------------------------------------
        // JSON message
        // -----------------------------------------------
        StaticJsonBuffer<200> jsonBuffer; // Memory pool for JSON object tree.
        JsonObject& jsonRoot = jsonBuffer.parseObject(msg);
        
        // Test if parsing succeeds.
        if (!jsonRoot.success()) {
          USESERIAL.println("parseObject() failed");
          return;
        }

        // Fetch values.
        //
        // Most of the time, you can rely on the implicit casts.
        // In other case, you can do root["time"].as<long>();
        const char* appMsgCmd = jsonRoot["cmd"];
        const char* appMsgValue = jsonRoot["value"];
  
        // message command format:
        // {"cmd": "", "value": ""}
        // -----------------------------------------------
        bool varBool = false;
        
        if (String(appMsgCmd) == "porta-open") {
          varBool = ( String(appMsgValue) == "true" );
          relay_Power(varBool);
        } else if (String(appMsgCmd) == "leds-on") {
          varBool = ( String(appMsgValue) == "true" );
          ledRing_Power(varBool);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main program
void mqttcom_handleClient() {   
  mqttclient_handle();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



WiFiClient espClient; // Creates the espClient object
PubSubClient MQTT(espClient); // Instantiate the MQTT Client by passing the espClient object
mqttclient_datacallback returnCb;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttclient_setCallback(mqttclient_datacallback cb) {
  returnCb = cb;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void data_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  String strTopic = String((char*)topic);

  USESERIAL.print("MQTT topic received: ");
  USESERIAL.println(strTopic);
    
  if(strTopic == SUBSCRIBE_COMMANDS) {
      //get the payload string received
      for(int i = 0; i < length; i++) {
         char c = (char)payload[i];
         msg += c;
      }

      returnCb(strTopic, msg);
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttclient_setup() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); //informs which broker and port should be connected
  MQTT.setCallback(data_callback);    //assigns callback function (called function when any information of one of the subscript topics arrives)
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void reconnectMQTT() {
  while (!MQTT.connected()) {
    USESERIAL.print("* Attempting to connect to the MQTT Broker: ");
    USESERIAL.println(BROKER_MQTT);
    
    if (MQTT.connect(ID_MQTT, BROKER_USER, BROKER_PWD)) {
        USESERIAL.println("Successfully connected to MQTT broker!");
        MQTT.subscribe(SUBSCRIBE_COMMANDS); 
    } else {
        USESERIAL.println("Failed to reconnect to broker.");
        USESERIAL.println("There will be a new connection attempt in 2s");
        delay(2000);
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CheckConnMQTT(void) {
  if (!MQTT.connected()) 
    reconnectMQTT(); //if there is no connection to the Broker, the connection is redone
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main program
void mqttclient_handle() {   
  //ensures WiFi connections and MQTT broker operation
  CheckConnMQTT();

  //keep-alive da comunicação com broker MQTT
  MQTT.loop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
