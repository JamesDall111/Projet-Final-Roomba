#ifndef GESTION_WIRELESS_H
#define GESTION_WIRELESS_H
 
#include <Arduino.h>
 
// Déclaration des structures
struct StructRobot {
  float tensionBatterie;
  int pourcentageBatterie;
  char modeActuel[16];
};
 
struct StructStation {
  bool contactsAlimentes;
  int correctionTrajectoire;
  char message[32];
};
 
// Partage des variables avec le reste du projet
extern StructRobot donneesRobot;
extern StructStation donneesStation;
 
// Fonctions disponibles pour le main.cpp
void initSansFilStation();
 
#endif