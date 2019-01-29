#include <config.h>
#include <PubSubClient.h> // Import the PubSubClient Library
#include <ArduinoJson.h>

typedef void (*mqttclient_datacallback) (String topic, String message);

//Prototypes
void mqttclient_setup();
void mqttclient_publish(String msg);
void mqttclient_handle();
void mqttclient_setCallback(mqttclient_datacallback cb);

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
void mqttclient_publish(String msg) {
  MQTT.publish(PUBLISH_DASHBOARD, msg.c_str());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main program
void mqttclient_handle() {   
  //ensures WiFi connections and MQTT broker operation
  CheckConnMQTT();

  //keep-alive da comunicação com broker MQTT
  MQTT.loop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
