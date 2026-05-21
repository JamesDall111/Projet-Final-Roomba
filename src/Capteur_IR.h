#ifndef CAPTEUR_IR_H
#define CAPTEUR_IR_H

#include <Arduino.h>

#define IR_RECEIVE_PIN 39

enum StationSignal {
  STATION_AUCUN,
  STATION_VU,
  STATION_INCONNU
};

extern StationSignal stationSignalConfirme;
extern unsigned long dernierSignalStationMs;

struct IRRawDebug {
  int rawlen;
  uint32_t t1;
  uint32_t t2;
  uint32_t t3;
  int nbBursts;
  uint32_t total;
};

void InitIR();
void LireIR();
IRRawDebug getIRRawDebug();

#endif