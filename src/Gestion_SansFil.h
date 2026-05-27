#ifndef GESTION_SANSFIL_H
#define GESTION_SANSFIL_H

#include <Arduino.h>

// Structure du Robot (Forcée compacte)
struct __attribute__((packed)) StructRobot {
  float tensionBatterie;
  int pourcentage;       
  char etat[16];         
};

// Structure de la Station (Forcée compacte)
struct __attribute__((packed)) StructStation {
  int rssi;                     
  int correctionTrajectoire;    
  bool contactsAlimentes;       
  char message[32];             
};

extern StructRobot donneesRobot;
extern StructStation donneesStation;
extern int rssiStation;

void initGestionSansFil();
void updateGestionSansFil();

#endif