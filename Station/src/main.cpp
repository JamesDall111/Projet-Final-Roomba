#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN  38   // LED RGB intégrée sur le board
#define NUM_LEDS 1

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(50);
}

void loop() {
  strip.setPixelColor(0, strip.Color(255, 0, 0)); // Rouge
  strip.show();
  delay(500);

  strip.setPixelColor(0, strip.Color(0, 0, 0)); // Éteint
  strip.show();
  delay(500);
}