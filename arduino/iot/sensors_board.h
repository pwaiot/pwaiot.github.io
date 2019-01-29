#include <config.h>

//mapping pinout GPIO - NodeMcu v3 / D1 Mini
#define ADC0  00 //Analog Pin 0
#define D0    16 //User / Wake // [NOTE: DON'T USE PIN 16 FOR NEOPIXELS]
#define D1    05 //~PWM //I2C - SCL
#define D2    04 //~PWM //I2C - SDA //D1 Mini - const BUILTIN_LED
#define D3    00 //~PWM //Flash
#define D4    02 //~PWM //TXD1 //NodeMcu v3 - LED_PIN
#define D5    14 //~PWM //SPI - SCK
#define D6    12 //~PWM //SPI - MISO
#define D7    13 //~PWM //SPI - MOSI //RXD2
#define D8    15 //~PWM //SPI - SS   //TXD2
#define D9    03 //~PWM //RXD0
#define D10   01 //~PWM //TXD0
//NodeMcu v3
#define SD3   10 //SDD3
#define SD2   09 //SDD2

#define LED_OFF HIGH
#define LED_ON  LOW

#define RELAY_OFF LOW
#define RELAY_ON  HIGH

#ifdef BOARD_NODEMCU
  #define LED_PIN             D4 //(D4) - NodeMcu v3 / (D2) - D1 Mini (const BUILTIN_LED)
  #define RELAY_A1_HEATER_PIN D2 //(D2) - NodeMcu v3 / (D4) - D1 Mini
#endif
#ifdef BOARD_D1_MINI
  #define LED_PIN             D2 //(D4) - NodeMcu v3 / (D2) - D1 Mini (const BUILTIN_LED)
  #define RELAY_A1_HEATER_PIN D4 //(D2) - NodeMcu v3 / (D4) - D1 Mini
#endif

#define LED_A1_STRIP_PIN    D5
#define RELAY_A2_LIGHT_PIN  D6
#define RELAY_A1_FAN_PIN    D7
#define RELAY_A2_FAN_PIN    D8
#define RELAY_A2_HEATER_PIN D3





#include <Adafruit_NeoPixel.h>
#define NUM_LEDS   60
//#define BRIGHTNESS 100

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_GRBW
//   NEO_RGBW
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_A1_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

//Prototypes
void ledstrip_setup();
void ledstrip_PowerLight(bool onOff);
void ledstrip_SetColor(byte red, byte green, byte blue, byte white);
byte ledstrip_GetColorRed();
byte ledstrip_GetColorGreen();
byte ledstrip_GetColorBlue();
byte ledstrip_GetColorWhite();





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








//private
void showStrip();
void setPixel(int Pixel, byte red, byte green, byte blue, byte white);
void setAll(byte red, byte green, byte blue, byte white);

#ifdef __AVR__
  #include <avr/power.h>
#endif

//initial values
byte pColorRed = 0;
byte pColorGreen = 0;
byte pColorBlue = 0;
byte pColorWhite = 255;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Helper function for rainbows
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ledstrip_setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  
  //strip.setBrightness(BRIGHTNESS); //setBrightness() was intended to be called once, in setup()
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  setAll(0, 0, 0, 0); //fix all to empty
  delay(100);
  rainbowCycle(1);//Flash rainbows at the begining
  USESERIAL.println("rainbowCycle");
  delay(50);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setPixel(int Pixel, byte red, byte green, byte blue, byte white) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue, white));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
   leds[Pixel].w = white;
 #endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setAll(byte red, byte green, byte blue, byte white) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue, white);
  }
  showStrip();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ledstrip_PowerLight(bool onOff) {
  if (onOff) {
    USESERIAL.println("ligando luz branca");
    setAll(0, 0, 0, 0); //clear all
    setAll(pColorRed, pColorGreen, pColorBlue, pColorWhite);
  } else {
    USESERIAL.println("desligando tudo");
    setAll(0, 0, 0, 0);
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ledstrip_SetColor(byte red, byte green, byte blue, byte white) {
  setAll(red, green, blue, white);
}

byte ledstrip_GetColorRed() {
  uint32_t myColor = strip.getPixelColor(0);
  return (uint8_t) ((myColor >> 16) & 0xFF);
}

byte ledstrip_GetColorGreen() {
  uint32_t myColor = strip.getPixelColor(0);
  return (uint8_t) ((myColor >> 8) & 0xFF);
}

byte ledstrip_GetColorBlue() {
  uint32_t myColor = strip.getPixelColor(0);
  return (uint8_t) ((myColor) & 0xFF);
}

byte ledstrip_GetColorWhite() {
  uint32_t myColor = strip.getPixelColor(0);
  return (uint8_t) ((myColor >> 24) & 0xFF);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
