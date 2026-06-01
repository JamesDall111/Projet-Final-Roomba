#ifndef CAPTEUR_IR_H
#define CAPTEUR_IR_H

#include <Arduino.h>
#include "Pins.h"

enum StationSignal {
  STATION_AUCUN,
  STATION_GAUCHE,
  STATION_DROITE,
  STATION_CENTRE,
  STATION_VU
};

extern StationSignal stationSignalConfirme;

void InitIR();
void LireIR();
void resetStationIR();

#endif