#ifndef DISTANCE_H
#define DISTANCE_H

#include <Arduino.h>
#include <VL53L0X.h>

#define NBR_CAPTEURS 2

extern VL53L0X capteurs[NBR_CAPTEURS];
extern int uiAdd[NBR_CAPTEURS];
extern int pins[NBR_CAPTEURS];
extern int distances[NBR_CAPTEURS];

void InitCapteur();
void LireDistances();
int IndexDistanceMin();
void scanI2C();

#endif