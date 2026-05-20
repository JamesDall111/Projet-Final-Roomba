#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

#include "Pins.h"
#include "Distance.h"

#define NBR_CAPTEURS 4

VL53L0X capteurs[NBR_CAPTEURS];

int adresses[NBR_CAPTEURS] = {0x31, 0x32, 0x33, 0x34};
int xshutPins[NBR_CAPTEURS] = {XSHUT_1, XSHUT_2, XSHUT_3, XSHUT_4};

void initCapteurs() {
  Wire.begin(SDA_PIN, SCL_PIN);

  for (int i = 0; i < NBR_CAPTEURS; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  delay(200);

  for (int i = 0; i < NBR_CAPTEURS; i++) {
    digitalWrite(xshutPins[i], HIGH);
    delay(200);

    if (capteurs[i].init()) {
      capteurs[i].setAddress(adresses[i]);
      capteurs[i].setTimeout(500);
    }
  }
}

int lireCapteur(int index) {
  int d = capteurs[index].readRangeSingleMillimeters();

  if (capteurs[index].timeoutOccurred()) return 9999;
  if (d <= 0) return 9999;
  if (d > 2000) return 2000;

  return d;
}