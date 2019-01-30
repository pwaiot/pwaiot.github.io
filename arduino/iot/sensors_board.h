#include <config.h>

// ----------------------------------------------
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

//LED_PIN = (D4) - NodeMcu v3 / (D2) - D1 Mini (const BUILTIN_LED)
#ifdef BOARD_NODEMCU
  #define LED_PIN      D4

  #define RELAY_PIN    D1
  #define LED_RING_PIN D2
#endif
#ifdef BOARD_D1_MINI
  #define LED_PIN      D2

  #define RELAY_PIN    D1
  #define LED_RING_PIN D3
#endif

#define RELAY_PIN    D1
#define LED_RING_PIN D2
// ----------------------------------------------

#define LED_RING_NUM_LEDS   8


#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_GRBW
//   NEO_RGBW
// Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_RING_NUM_LEDS, LED_RING_PIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_RING_NUM_LEDS, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

//prototypes
void sensors_Setup();
void relay_Power(bool onOff);
void ledRing_Power(bool onOff);

//private
void relay_setup();
void ledstrip_setup();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sensors_Setup() {
  pinMode(RELAY_PIN, OUTPUT); //set RELAY_PIN as output
  digitalWrite(RELAY_PIN, RELAY_OFF);

  //pinMode(LED_RING_PIN, OUTPUT);
  //digitalWrite(LED_RING_PIN, RELAY_OFF);

  ledstrip_setup();
  relay_setup();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void relay_Power(bool onOff) {
/*  if (onOff == true) {
    //USESERIAL.println("RELAY_ON");
    digitalWrite(RELAY_PIN, RELAY_ON);
  } else {
    //USESERIAL.println("RELAY_OFF");
    digitalWrite(RELAY_PIN, RELAY_OFF);
  } */

  digitalWrite(RELAY_PIN, RELAY_ON);
  delay(1000); //wait 1s for power off
  digitalWrite(RELAY_PIN, RELAY_OFF);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void relay_setup() {
  //flash relay
  relay_Power(true);
  delay(500);
  relay_Power(true);
  delay(500);
  relay_Power(true);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//private
void showStrip();
void setPixel(int Pixel, byte red, byte green, byte blue);
void setAll(byte red, byte green, byte blue);

#ifdef __AVR__
  #include <avr/power.h>
#endif

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
  setAll(0, 0, 0); //fix all to empty
  delay(100);
  rainbowCycle(1);//Flash rainbows at the begining
  USESERIAL.println("rainbowCycle");
  delay(1000);
  setAll(0, 0, 0); //fix all to empty
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
void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
   //leds[Pixel].w = white;
 #endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < LED_RING_NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ledRing_Power(bool onOff) {
/*  if (onOff) {
    USESERIAL.println("ligando luz branca");
    setAll(0, 0, 0); //clear all
  } else {
    USESERIAL.println("desligando tudo");
    setAll(0, 0, 0);
  } */
  setAll(0, 0, 0); //fix all to empty
  delay(100);
  rainbowCycle(1);//Flash rainbows at the begining
  delay(1000);
  setAll(0, 0, 0); //fix all to empty
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
