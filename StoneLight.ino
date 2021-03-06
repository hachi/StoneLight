
#define ENCODER_DO_NOT_USE_INTERRUPTS

#include <Adafruit_NeoPixel.h>
#include <EnableInterrupt.h>
#include <Encoder.h>
#include <EEPROM.h>

#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(28, PIN, NEO_GRB + NEO_KHZ800);
Encoder myEnc(7, 8);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

long pos;

unsigned char mode;
unsigned char brightness = 127;

void interruptFunction() {
  long newpos = myEnc.read();
  brightness += newpos - pos;
  pos = newpos;
}

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  pos = myEnc.read();

  enableInterrupt(7, interruptFunction, CHANGE);
  enableInterrupt(8, interruptFunction, CHANGE);

  EEPROM.get(0, mode);
  EEPROM.update(0, (mode + 1) % 12);
}

void loop() {
  long newpos = myEnc.read();
  brightness += pos - newpos;
  pos = newpos;

  switch (mode) {
    case 0:
      colorWipe(strip.Color(brightness, brightness, brightness), 50); // White
      break;
    case 1:
      colorWipe(strip.Color(brightness, 0, 0), 50); // Red
      break;
    case 2:
      colorWipe(strip.Color(0, brightness, 0), 50); // Green
      break;
    case 3:
      colorWipe(strip.Color(0, 0, brightness), 50); // Blue
      break;
    case 4:
      theaterChase(strip.Color(brightness, brightness, brightness), 50); // White
      break;
    case 5:
      theaterChase(strip.Color(brightness, 0, 0), 50); // Red
      break;
    case 6:
      theaterChase(strip.Color(0, brightness, 0), 50); // Green
      break;
    case 7:
      theaterChase(strip.Color(0, 0, brightness), 50); // Blue
      break;
    case 8:
      theaterChase(strip.Color(brightness, brightness / 4, 0), 50); // Fire-ish
      break;
    case 9:
      rainbow(20);
      break;
    case 10:
      rainbowCycle(20);
      break;
    case 11:
      theaterChaseRainbow(50);
      break;
  }

  EEPROM.update(0, mode);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
