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
