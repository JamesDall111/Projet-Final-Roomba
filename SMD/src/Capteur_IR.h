#ifndef CAPTEUR_IR_H
#define CAPTEUR_IR_H

#include <Arduino.h>

#define IR_RECEIVE_PIN 15

// Télécommande
const uint16_t REMOTE_ADDRESS = 0x4587;

const uint8_t CMD_AVANCE = 0x28;
const uint8_t CMD_DROITE = 0x2A;
const uint8_t CMD_GAUCHE = 0x2E;
const uint8_t CMD_RECULE = 0x2C;
const uint8_t CMD_MODE   = 0x17;

// Station
const uint16_t STATION_ADDRESS = 0x35;
const uint8_t STATION_COMMAND  = 0xCA;

enum ModeRobot
{
  MODE_MANUEL,
  MODE_AUTOMATIQUE,
  MODE_CHERCHE_BATTERIE
};

extern ModeRobot modeActuel;
extern bool stationDetectee;

void InitIR();
void LireIR();

bool commandeValide(uint8_t cmd);
void afficherCommande(uint8_t cmd);
bool signalStationValide(uint16_t adresse, uint8_t cmd);

#endif