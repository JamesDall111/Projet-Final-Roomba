#include <Arduino.h>
#include "Pins.h"
#include "Batterie.h"

const float ADC_MAX = 4095.0;
const float ADC_REF = 3.3;
const float DIVISEUR = 5.545;

const float BAT_MIN = 12.0;
const float BAT_MAX = 16.8;

void initBatterie() {
  pinMode(BAT_SENSOR, INPUT);
  analogReadResolution(12);
}

float lireTensionBatterie() {
  long somme = 0;

  for (int i = 0; i < 30; i++) {
    somme += analogRead(BAT_SENSOR);
    delay(2);
  }

  float adc = somme / 30.0;
  float tensionADC = (adc / ADC_MAX) * ADC_REF;

  return tensionADC * DIVISEUR;
}

int lirePourcentageBatterie(float tension) {
  int pct = map(tension * 100, BAT_MIN * 100, BAT_MAX * 100, 0, 100);

  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;

  return pct;
}