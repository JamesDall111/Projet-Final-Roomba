#ifndef GESTION_SANSFIL_H
#define GESTION_SANSFIL_H

#include <Arduino.h>

// Structure du Robot (Doit être le miroir exact de la Station)
struct StructRobot {
  float tensionBatterie;
  int pourcentage;       
  char etat[16];         
};

// Structure de la Station reçue en retour
struct StructStation {
  bool contactsAlimentes;
  int correctionTrajectoire;
  char message[32];
};

// Partage des variables avec le reste des fichiers du Robot (ex: WebControl.cpp)
extern StructRobot donneesRobot;
extern StructStation donneesStation;

// Fonctions principales
void initGestionSansFil();
void updateGestionSansFil();

#endif