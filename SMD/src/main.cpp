#include <Arduino.h>
#include <IRremote.hpp>
#include "Distance.h"

#define IR_RECEIVE_PIN 15

#define CAPTEUR_GAUCHE 0
#define CAPTEUR_DROITE 1

const int DISTANCE_OBSTACLE = 150;

// Adresse de la télécommande
const uint16_t REMOTE_ADDRESS = 0x4587;

// Commandes
const uint8_t CMD_AVANCE = 0x28;
const uint8_t CMD_DROITE = 0x2A;
const uint8_t CMD_GAUCHE = 0x2E;
const uint8_t CMD_RECULE = 0x2C;
const uint8_t CMD_MODE   = 0x17;

enum ModeRobot
{
  MODE_MANUEL,
  MODE_AUTOMATIQUE
};

ModeRobot modeActuel = MODE_MANUEL;

// -------------------------------------------------------------------------------------------------
// Retourne true si la commande fait partie de celles qu'on accepte
// -------------------------------------------------------------------------------------------------
bool commandeValide(uint8_t cmd)
{
  return (cmd == CMD_AVANCE ||
          cmd == CMD_DROITE ||
          cmd == CMD_GAUCHE ||
          cmd == CMD_RECULE ||
          cmd == CMD_MODE);
}

// -------------------------------------------------------------------------------------------------
// Affichage clair de la commande
// -------------------------------------------------------------------------------------------------
void afficherCommande(uint8_t cmd)
{
  if (cmd == CMD_AVANCE)
  {
    Serial.println(">>> AVANCE");
  }
  else if (cmd == CMD_DROITE)
  {
    Serial.println(">>> DROITE");
  }
  else if (cmd == CMD_GAUCHE)
  {
    Serial.println(">>> GAUCHE");
  }
  else if (cmd == CMD_RECULE)
  {
    Serial.println(">>> RECULE");
  }
  else if (cmd == CMD_MODE)
  {
    Serial.println(">>> CHANGER MODE");
  }
  else
  {
    Serial.println(">>> COMMANDE INCONNUE");
  }
}

// -------------------------------------------------------------------------------------------------
// Mode automatique sans moteur : affiche juste ce que le robot ferait
// -------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(500);

  InitCapteur();

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Pret");
  Serial.println("Mode MANUEL");
  Serial.println("----------------------------");
}

// -------------------------------------------------------------------------------------------------
// Loop
// -------------------------------------------------------------------------------------------------
void loop()
{
  if (IrReceiver.decode())
  {
    uint16_t adresse = IrReceiver.decodedIRData.address;
    uint8_t cmd = IrReceiver.decodedIRData.command;
    bool repetition = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

    // On ignore les repeats pour éviter le spam
    if (!repetition)
    {
      // On garde un petit filtrage :
      // - on garde seulement les commandes qui nous intéressent
      // - on garde aussi la bonne adresse
      if (commandeValide(cmd) && adresse == REMOTE_ADDRESS)
      {
        Serial.print("Adresse = 0x");
        Serial.println(adresse, HEX);

        Serial.print("Commande = 0x");
        Serial.println(cmd, HEX);

        afficherCommande(cmd);

        if (cmd == CMD_MODE)
        {
          if (modeActuel == MODE_MANUEL)
          {
            modeActuel = MODE_AUTOMATIQUE;
            Serial.println("=== MODE AUTOMATIQUE ===");
          }
          else
          {
            modeActuel = MODE_MANUEL;
            Serial.println("=== MODE MANUEL ===");
          }
        }
        else if (modeActuel == MODE_MANUEL)
        {
          // En mode manuel on affiche juste la direction détectée
          // sans moteur ici
        }

        Serial.println("----------------------------");
      }
      else
      {
        // On garde un peu de bruit, mais propre
        // sans afficher les numéros de protocoles inutiles
        if (cmd != 0x00)
        {
          Serial.print("Bruit / autre signal detecte");
          Serial.print(" | Addr=0x");
          Serial.print(adresse, HEX);
          Serial.print(" | Cmd=0x");
          Serial.println(cmd, HEX);
          Serial.println("----------------------------");
        }
      }
    }

    IrReceiver.resume();
  }

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