#include <config.h> //copy "config.example.h" to "config.h" and edit it
#include <Time.h>
#include <TimeLib.h>
//#include "mqtt_brocker.h" //server mode
#include "mqtt_client.h" //client mode

#include <ArduinoJson.h>

//Global variables and objects
String sensorA1Temp = "0"; //A1 Temp Sensor
String sensorA2Temp = "0"; //A2 Temp Sensor

//Prototypes
void mqttcom_setup();
void mqttcom_initialValues();
void mqttcom_sendSensors();
void mqttcom_handleClient();

//private
void initMQTT();
void mqttcom_callback(String topic, String msg);
void refreshMQTTSensors();
void FormatAndSendMQTTMsg(String cmd, String value, bool valueIsString = true);

void readAndSendA1Temp();
void readAndSendA2Temp();
void readAndSendRelayLight();
void readAndSendLedStrip();
void readAndSendA1Color();
void readAndSendA1Fan();
void readAndSendA2Fan();
void readAndSendA1Thermo();
void readAndSendA2Thermo();
String getFormatedDateTime();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String queueMQTT = "";
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttcom_setup() {
    initMQTT();
    sensors_Setup();
    //initial values is set by main program //refreshMQTTSensors(); //initial values from sensors
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
        
        if (String(appMsgCmd) == "a1-luz-on") {
          varBool = ( String(appMsgValue) == "true" );
          sensorA1_Power(varBool);
        } else if (String(appMsgCmd) == "a2-luz-on") {
          varBool = ( String(appMsgValue) == "true" );
          sensorA2_Power(varBool);

        } else if (String(appMsgCmd) == "a1-color") {
          byte cRed   = jsonRoot["value"][0] | 0;
          byte cGreen = jsonRoot["value"][1] | 0;
          byte cBlue  = jsonRoot["value"][2] | 0;
          byte cWhite = jsonRoot["value"][3] | 0;
          sensorA1_SetColor(cRed, cGreen, cBlue, cWhite);

        } else if (String(appMsgCmd) == "a1-fan-on") {
          varBool = ( String(appMsgValue) == "true" );
          sensorA1Fan_Power(varBool);
        } else if (String(appMsgCmd) == "a2-fan-on") {
          varBool = ( String(appMsgValue) == "true" );
          sensorA2Fan_Power(varBool);
        
        } else if (String(appMsgCmd) == "a1-therm-on") {
          varBool = ( String(appMsgValue) == "true" );
          sensorA1Thermo_Power(varBool);
        } else if (String(appMsgCmd) == "a2-therm-on") {
          varBool = ( String(appMsgValue) == "true" );
          sensorA2Thermo_Power(varBool);

        } else if (String(appMsgCmd) == "initvalues") {
          //send initial values for fast refresh
          //read and sendo to server
          refreshMQTTSensors();
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FormatAndSendMQTTMsg(String cmd, String value, bool valueIsString) {
  String msg = "";
  String dateTime = getFormatedDateTime();

  if (valueIsString) {
    msg = "{\"cmd\":\""+cmd+"\",\"value\":\""+value+"\",\"datetime\":\""+dateTime+"\"}";
  } else {
    msg = "{\"cmd\":\""+cmd+"\",\"value\":"+value+",\"datetime\":\""+dateTime+"\"}";
  }

  //msg = "{\"msg\":["+ msg + "], \"datetime\":\""+dateTime+"\"}";

  USESERIAL.print("- FormatAndSendMQTTMsg: msg = ");
  USESERIAL.println(msg);
  mqttclient_publish(msg);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendA1Temp() {
  sensorA1Temp = sensorA1_GetTemp(); //read from sensor
  FormatAndSendMQTTMsg("a1-temp", sensorA1Temp);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendA2Temp() {
  sensorA2Temp = sensorA2_GetTemp(); //read from sensor
  FormatAndSendMQTTMsg("a2-temp", sensorA2Temp);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendA1Color() {
  String cRed   = String(sensorA1_GetColorRed());
  String cGreen = String(sensorA1_GetColorGreen());
  String cBlue  = String(sensorA1_GetColorBlue());
  String cWhite = String(sensorA1_GetColorWhite());
  String color = "[\""+cRed+"\",\""+cGreen+"\",\""+cBlue+"\",\""+cWhite+"\"]";

  FormatAndSendMQTTMsg("a1-color", color, false); //format value without ""
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendRelayLight() {
  bool varBool = sensorA2_IsOn();
  String varStr = varBool ? "true" : "false";

  FormatAndSendMQTTMsg("a2-luz-on", varStr);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendLedStrip() {
  bool varBool = sensorA1_IsOn();
  String varStr = varBool ? "true" : "false";

  FormatAndSendMQTTMsg("a1-luz-on", varStr);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendA1Fan() {
  bool varBool = sensorA1Fan_IsOn();
  String varStr = varBool ? "true" : "false";

  FormatAndSendMQTTMsg("a1-fan-on", varStr);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendA2Fan() {
  bool varBool = sensorA2Fan_IsOn();
  String varStr = varBool ? "true" : "false";

  FormatAndSendMQTTMsg("a2-fan-on", varStr);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendA1Thermo() {
  bool varBool = sensorA1Thermo_IsOn();
  String varStr = varBool ? "true" : "false";

  FormatAndSendMQTTMsg("a1-therm-on", varStr);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readAndSendA2Thermo() {
  bool varBool = sensorA2Thermo_IsOn();
  String varStr = varBool ? "true" : "false";

  FormatAndSendMQTTMsg("a2-therm-on", varStr);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void refreshMQTTSensors() {
  readAndSendA1Temp();
  delay(250); //delay for mqtt brocker
  readAndSendA2Temp();
  delay(250); //delay for mqtt brocker
  
  readAndSendLedStrip();
  delay(250); //delay for mqtt brocker
  readAndSendA1Color();
  delay(250); //delay for mqtt brocker
  readAndSendRelayLight();
  delay(250); //delay for mqtt brocker

  readAndSendA1Fan();
  delay(250); //delay for mqtt brocker
  readAndSendA2Fan();
  delay(250); //delay for mqtt brocker
  
  readAndSendA1Thermo();
  delay(250); //delay for mqtt brocker
  readAndSendA2Thermo();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttcom_sendSensors() {   
    //sends the status of all sensors to the Broker in the expected protocol
    refreshMQTTSensors();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main program
void mqttcom_handleClient() {   
  mqttclient_handle();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mqttcom_initialValues() {
  //write initial values
  sensorA2_Power(true);  
  sensorA1_Power(true);

  //read and sendo to server
  refreshMQTTSensors();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String formatDigits(int digits) {
  String out = "";
  if(digits < 10)
    out = '0';
  out += String(digits);

  return out;
}

String getFormatedDateTime() {
   String str = String(year()) + "-" + formatDigits(month()) + "-" + formatDigits(day()) + " " + formatDigits(hour()) + ":" + formatDigits(minute()) + ":" + formatDigits(second());
   return str;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
