#include <Arduino.h>
#include "Distance.h"
#include "Capteur_IR.h"

#define CAPTEUR_GAUCHE 0
#define CAPTEUR_DROITE 1

const int DISTANCE_OBSTACLE = 150;

void modeAutomatique()
{
  LireDistances();

  Serial.print("Capteur 0 : ");
  Serial.print(distances[0]);
  Serial.println(" mm");

  Serial.print("Capteur 1 : ");
  Serial.print(distances[1]);
  Serial.println(" mm");

  if (distances[0] >= 9000 || distances[1] >= 9000)
  {
    Serial.println("Erreur capteur / aucun signal fiable");
    Serial.println("----------------------------");
    return;
  }

  int indexMin = IndexDistanceMin();
  int distanceMin = distances[indexMin];

  Serial.print("Distance minimale : ");
  Serial.print(distanceMin);
  Serial.println(" mm");

  if (distanceMin < DISTANCE_OBSTACLE)
  {
    if (indexMin == CAPTEUR_GAUCHE)
    {
      Serial.println("AUTO -> obstacle a gauche");
      Serial.println("AUTO -> tourner a droite");
    }
    else
    {
      Serial.println("AUTO -> obstacle a droite");
      Serial.println("AUTO -> tourner a gauche");
    }
  }
  else
  {
    Serial.println("AUTO -> chemin libre");
    Serial.println("AUTO -> avancer");
  }

  Serial.println("----------------------------");
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  InitCapteur();
  InitIR();

  Serial.println("Pret");
  Serial.println("Mode MANUEL");
  Serial.println("----------------------------");
}

void loop()
{
  LireIR();

  if (modeActuel == MODE_AUTOMATIQUE)
  {
    modeAutomatique();
    delay(300);
  }
  else
  {
    delay(50);
  }
}