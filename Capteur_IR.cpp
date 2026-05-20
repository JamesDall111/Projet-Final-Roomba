#include <Arduino.h>
#include <IRremote.hpp>
#include "Capteur_IR.h"

ModeRobot modeActuel = MODE_MANUEL;
bool stationDetectee = false;

bool commandeValide(uint8_t cmd)
{
  return (cmd == CMD_AVANCE ||
          cmd == CMD_DROITE ||
          cmd == CMD_GAUCHE ||
          cmd == CMD_RECULE ||
          cmd == CMD_MODE);
}

bool signalStationValide(uint16_t adresse, uint8_t cmd)
{
  return (adresse == STATION_ADDRESS && cmd == STATION_COMMAND);
}

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

void InitIR()
{
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR pret");
}

void LireIR()
{
  stationDetectee = false;

  if (!IrReceiver.decode())
  {
    return;
  }

  uint16_t adresse = IrReceiver.decodedIRData.address;
  uint8_t cmd = IrReceiver.decodedIRData.command;
  bool repetition = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

  // 1) Détection station
  if (signalStationValide(adresse, cmd))
  {
    stationDetectee = true;

    Serial.print("STATION detectee");
    Serial.print(" | Addr=0x");
    Serial.print(adresse, HEX);
    Serial.print(" | Cmd=0x");
    Serial.println(cmd, HEX);

    if (repetition)
    {
      Serial.println(">>> SIGNAL STATION REPETE");
    }

    Serial.println("----------------------------");
    IrReceiver.resume();
    return;
  }

  // 2) Télécommande
  if (adresse == REMOTE_ADDRESS && commandeValide(cmd))
  {
    Serial.print("Adresse = 0x");
    Serial.println(adresse, HEX);

    Serial.print("Commande = 0x");
    Serial.println(cmd, HEX);

    if (repetition)
    {
      Serial.println(">>> REPEAT");
    }

    afficherCommande(cmd);

    // Pour MODE, on évite juste le changement de mode en boucle
    if (cmd == CMD_MODE && !repetition)
    {
      if (modeActuel == MODE_MANUEL)
      {
        modeActuel = MODE_AUTOMATIQUE;
        Serial.println("=== MODE AUTOMATIQUE ===");
      }
      else if (modeActuel == MODE_AUTOMATIQUE)
      {
        modeActuel = MODE_CHERCHE_BATTERIE;
        Serial.println("=== MODE CHERCHE BATTERIE ===");
      }
      else
      {
        modeActuel = MODE_MANUEL;
        Serial.println("=== MODE MANUEL ===");
      }
    }

    Serial.println("----------------------------");
  }
  else
  {
    // 3) Bruit / autres signaux
    // On garde un peu d'info, sans trop filtrer
    if (cmd != 0x00 || adresse != 0x0000)
    {
      Serial.print("Bruit / autre signal");
      Serial.print(" | Addr=0x");
      Serial.print(adresse, HEX);
      Serial.print(" | Cmd=0x");
      Serial.println(cmd, HEX);

      if (repetition)
      {
        Serial.println(">>> REPEAT BRUIT");
      }

      Serial.println("----------------------------");
    }
  }

  IrReceiver.resume();
}