#include "pinout_board.h"
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
