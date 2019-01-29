#include <config.h> //copy "config.example.h" to "config.h" and edit it
#include <WebSocketsServer.h>
#include <MQTTbroker.h>

typedef void (*mqttbrocker_datacallback) (String topic, String message);

//Prototypes
void mqttbrocker_setup();
void mqttbrocker_publish(String msg);
void mqttbrocker_handle();
void mqttbrocker_setCallback(mqttbrocker_datacallback cb);

WebSocketsServer webSocket = WebSocketsServer(80,"mqtt","mqtt"); //WebSocketsServer(uint16_t port, String origin = "", String protocol = "arduino");
MQTTbroker Broker = MQTTbroker(&webSocket);
mqttbrocker_datacallback returnCb;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttbrocker_setCallback(mqttbrocker_datacallback cb) {
  returnCb = cb;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void data_callback(String topic_name, uint8_t *payload, uint8_t length_payload) {
  char data_str[length_payload+1];
  os_memcpy(data_str, payload, length_payload);
  data_str[length_payload] = '\0';

  USESERIAL.print("received topic '");
  USESERIAL.print(topic_name);
  USESERIAL.print("' with data '");
  USESERIAL.print(data_str);
  USESERIAL.println("'");

  returnCb(topic_name, data_str);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED:
      if (Broker.clientIsConnected(num)) Broker.disconnect(num);                
      USESERIAL.println("ws: disconnected");
      break;            

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        USESERIAL.printf("ws: [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
    
        // send message to client
        webSocket.sendTXT(num, "Connected");
        USESERIAL.println("ws: sending connected message");
      }
      break;

    case WStype_TEXT:
      USESERIAL.printf("ws: [%u] get Text: %s\n", num, payload);

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;

    case WStype_BIN:
      Broker.parsing(num, payload, lenght);
      USESERIAL.println("ws: WStype_BIN");
      break;
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttbrocker_setup()
{
  USESERIAL.println("Starting webSocket");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  USESERIAL.println("Starting MQTT broker");
  Broker.begin();
  Broker.setCallback(data_callback);
  Broker.subscribe(SUBSCRIBE_COMMANDS);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttbrocker_publish(String msg)
{
  //Broker.publish(PUBLISH_DASHBOARD, msg);
  //Broker.publish((prefix + "/" + deviceID + "/config").c_str(), (uint8_t*)thing_config[i].c_str(), thing_config[i].length());
  Broker.publish(PUBLISH_DASHBOARD, (uint8_t*)msg.c_str(), msg.length());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttbrocker_handle() {
  webSocket.loop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//myMQTTBroker myBroker; //custom  myBroker;
//uMQTTBroker myBroker;
/*
 * Custom broker class with overwritten callback functions
 */
/*
class myMQTTBroker: public uMQTTBroker
{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      USESERIAL.println("MQTT Brocker -> "+addr.toString()+" connected");
      return true;
    }
    
    virtual bool onAuth(String username, String password) {
      USESERIAL.println("MQTT Brocker -> Username/Password: "+username+"/"+password);
      return true;
    }

    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      
      USESERIAL.println("MQTT Brocker -> received topic '"+topic+"' with data '"+(String)data_str+"'");
    }
};
*/
