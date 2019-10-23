/*
 neo pixel test
 please install the Adafruit library first!
 October 2019, Toppppp
*/

#include <Adafruit_NeoPixel.h>

#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  pixels.begin(); 
  delay(10);
  /* Fill all pixels with black */
  for(int i=0; i<pixels.numPixels(); i++) { // For each pixel in pixels...
    pixels.setPixelColor(i, pixels.Color(0,   0,   0));         //  Set pixel's color (in RAM)
  }
  pixels.show();
  pixels.show(); // This library has a bug for the first LED, so we need to invoke this function twice

}

void loop() {

  
  delay(1000);
  for(int i=0; i<pixels.numPixels(); i++) { 
    pixels.setPixelColor(i, pixels.Color(255,   0,   0));         
  }
  pixels.show();
  delay(1000);
  for(int i=0; i<pixels.numPixels(); i++) { 
    pixels.setPixelColor(i, pixels.Color(0,   255,   0));         
  }
  pixels.show();
  delay(1000);
  for(int i=0; i<pixels.numPixels(); i++) { 
    pixels.setPixelColor(i, pixels.Color(0,   0,   255));        
  }
  pixels.show();
  delay(1000);
  for(int i=0; i<pixels.numPixels(); i++) { 
    pixels.setPixelColor(i, pixels.Color(0,   0,   0));       
  }
  pixels.show();
}

