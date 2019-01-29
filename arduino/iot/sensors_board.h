#include <config.h>
#include "pinout_board.h"
#include "led_strip.h"

//prototypes
void sensors_Setup();

void sensorA2_Power(bool onOff);
bool sensorA2_IsOn();

void sensorA1_Power(bool onOff);
bool sensorA1_IsOn();

void sensorA1_SetColor(byte red, byte green, byte blue, byte white);
byte sensorA1_GetColorRed();
byte sensorA1_GetColorGreen();
byte sensorA1_GetColorBlue();
byte sensorA1_GetColorWhite();

String sensorA1_GetTemp();
String sensorA2_GetTemp();

void sensorA1Fan_Power(bool onOff);
bool sensorA1Fan_IsOn();

void sensorA2Fan_Power(bool onOff);
bool sensorA2Fan_IsOn();

void sensorA1Thermo_Power(bool onOff);
bool sensorA1Thermo_IsOn();

void sensorA2Thermo_Power(bool onOff);
bool sensorA2Thermo_IsOn();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorTemp_Setup();

int localSensorA1Temp = 0;
int localSensorA2Temp = 0;
bool localSensorLedStripOn = false;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensors_Setup() {
  //led light
  pinMode(RELAY_A2_LIGHT_PIN, OUTPUT); //set RELAY_A2_LIGHT_PIN as output
  digitalWrite(RELAY_A2_LIGHT_PIN, RELAY_OFF);

  //fans
  pinMode(RELAY_A1_FAN_PIN, OUTPUT);
  digitalWrite(RELAY_A1_FAN_PIN, RELAY_OFF);
  
  pinMode(RELAY_A2_FAN_PIN, OUTPUT);
  digitalWrite(RELAY_A2_FAN_PIN, RELAY_OFF);

  //heaters
  pinMode(RELAY_A1_HEATER_PIN, OUTPUT);
  digitalWrite(RELAY_A1_HEATER_PIN, RELAY_OFF);
  
  pinMode(RELAY_A2_HEATER_PIN, OUTPUT);
  digitalWrite(RELAY_A2_HEATER_PIN, RELAY_OFF);

  //led strip
  ledstrip_setup();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorTemp_Setup() {
  /////////////ledstrip_setup();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool sensorA2_IsOn() {
  int val = digitalRead(RELAY_A2_LIGHT_PIN);
  //USESERIAL.print("sensorA2_IsOn");
  //USESERIAL.println(val);

  return (val == RELAY_ON);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool sensorA1Fan_IsOn() {
  int val = digitalRead(RELAY_A1_FAN_PIN);
  //USESERIAL.print("sensorA1Fan_IsOn");
  //USESERIAL.println(val);

  return (val == RELAY_ON);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool sensorA2Fan_IsOn() {
  int val = digitalRead(RELAY_A2_FAN_PIN);
  //USESERIAL.print("sensorA2Fan_IsOn");
  //USESERIAL.println(val);

  return (val == RELAY_ON);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool sensorA1Thermo_IsOn() {
  int val = digitalRead(RELAY_A1_HEATER_PIN);
  //USESERIAL.print("sensorA1Thermo_IsOn");
  //USESERIAL.println(val);

  return (val == RELAY_ON);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool sensorA2Thermo_IsOn() {
  int val = digitalRead(RELAY_A2_HEATER_PIN);
  //USESERIAL.print("sensorA2Thermo_IsOn");
  //USESERIAL.println(val);

  return (val == RELAY_ON);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool sensorA1_IsOn() {
  return localSensorLedStripOn;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorA1_SetColor(byte red, byte green, byte blue, byte white) {
  ledstrip_SetColor(red, green, blue, white);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
byte sensorA1_GetColorRed() {
  return ledstrip_GetColorRed();
}
byte sensorA1_GetColorGreen() {
  return ledstrip_GetColorGreen();
}
byte sensorA1_GetColorBlue() {
  return ledstrip_GetColorBlue();
}
byte sensorA1_GetColorWhite() {
  return ledstrip_GetColorWhite();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorA2_Power(bool onOff) {
  if (onOff == true) {
    //USESERIAL.println("RELAY_ON");
    digitalWrite(RELAY_A2_LIGHT_PIN, RELAY_ON);
  } else {
    //USESERIAL.println("RELAY_OFF");
    digitalWrite(RELAY_A2_LIGHT_PIN, RELAY_OFF);
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorA1Fan_Power(bool onOff) {
  int value = 0;
  if (onOff == true) {
    //USESERIAL.println("RELAY_A1_FAN_ON");
    value = RELAY_ON;
  } else {
    //USESERIAL.println("RELAY_A1_FAN_OFF");
    value = RELAY_OFF;
  }

  digitalWrite(RELAY_A1_FAN_PIN, value);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorA2Fan_Power(bool onOff) {
  int value = 0;
  if (onOff == true) {
    //USESERIAL.println("RELAY_A2_FAN_ON");
    value = RELAY_ON;
  } else {
    //USESERIAL.println("RELAY_A2_FAN_OFF");
    value = RELAY_OFF;
  }

  digitalWrite(RELAY_A2_FAN_PIN, value);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorA1Thermo_Power(bool onOff) {
  int value = 0;
  if (onOff == true) {
    //USESERIAL.println("RELAY_A1_HEATER_ON");
    value = RELAY_ON;
  } else {
    //USESERIAL.println("RELAY_A1_HEATER_OFF");
    value = RELAY_OFF;
  }

  digitalWrite(RELAY_A1_HEATER_PIN, value);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorA2Thermo_Power(bool onOff) {
  int value = 0;
  if (onOff == true) {
    //USESERIAL.println("RELAY_A2_HEATER_ON");
    value = RELAY_ON;
  } else {
    //USESERIAL.println("RELAY_A2_HEATER_OFF");
    value = RELAY_OFF;
  }

  digitalWrite(RELAY_A2_HEATER_PIN, value);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensorA1_Power(bool onOff) {
  localSensorLedStripOn = onOff;
  ledstrip_PowerLight(onOff);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String sensorA1_GetTemp() {
    localSensorA1Temp++;

    //char cstr[16];
    //itoa(sensor, cstr, 10);
    //sprintf(cstr, "%05d", num);
    //sprintf_P(cstr, (PGM_P)F("%02d:%02d:%02d"), hours, minutes, seconds);
    //sprintf_P(cstr, (PGM_P)F("%05d"), sensor);

    //convert to char* and format to 5 digits
    //sprintf(cstr, "%05d", sensor);
    //return String(cstr);

    return String(localSensorA1Temp);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String sensorA2_GetTemp() {
    localSensorA2Temp++;

    localSensorA2Temp++; //plus
    
    return String(localSensorA2Temp);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
