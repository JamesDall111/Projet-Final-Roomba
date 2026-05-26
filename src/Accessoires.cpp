#include <Arduino.h>
#include "Pins.h"
#include "Accessoires.h"

bool vacState = false;
bool mainState = false;
bool gaucheState = false;
bool droiteState = false;

void initAccessoires() {
  pinMode(VAC_ON, OUTPUT);
  pinMode(MAIN_ON, OUTPUT);
  pinMode(M_GAUCHE, OUTPUT);
  pinMode(M_DROITE, OUTPUT);

  accessoiresOff();
}

void appliquerAccessoires() {
  digitalWrite(VAC_ON, vacState ? HIGH : LOW);
  digitalWrite(MAIN_ON, mainState ? HIGH : LOW);
  digitalWrite(M_GAUCHE, gaucheState ? HIGH : LOW);
  digitalWrite(M_DROITE, droiteState ? HIGH : LOW);
}

void accessoiresOn() {
  vacState = true;
  mainState = true;
  gaucheState = true;
  droiteState = true;
  appliquerAccessoires();
}

void accessoiresOff() {
  vacState = false;
  mainState = false;
  gaucheState = false;
  droiteState = false;
  appliquerAccessoires();
}