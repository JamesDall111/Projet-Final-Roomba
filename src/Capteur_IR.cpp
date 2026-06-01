#include <Arduino.h>
#include "Capteur_IR.h"
#include "WebControl.h"

#define CONFIRMATIONS_NECESSAIRES 2
#define TIMEOUT_SIGNAL_MS 2500

StationSignal stationSignalConfirme = STATION_AUCUN;
StationSignal dernierTypeDetecte = STATION_AUCUN;

int compteurConfirmation = 0;
unsigned long dernierSignalStationMs = 0;

void resetStationIR() {
  stationSignalConfirme = STATION_AUCUN;
  dernierTypeDetecte = STATION_AUCUN;
  compteurConfirmation = 0;
}

StationSignal lireDirectionIR() {
  bool gauche = digitalRead(IR_AG) == LOW; // TSOP = LOW quand il détecte
  bool droite = digitalRead(IR_AD) == LOW;

  if (gauche && droite) return STATION_CENTRE;
  if (gauche && !droite) return STATION_GAUCHE;
  if (!gauche && droite) return STATION_DROITE;

  return STATION_AUCUN;
}

void confirmerSignalStation(StationSignal type) {
  if (type == STATION_AUCUN) return;

  if (type == dernierTypeDetecte) {
    compteurConfirmation++;
  } else {
    dernierTypeDetecte = type;
    compteurConfirmation = 1;
  }

  if (compteurConfirmation >= CONFIRMATIONS_NECESSAIRES) {
    stationSignalConfirme = type;
    dernierSignalStationMs = millis();
  }
}

void InitIR() {
  pinMode(IR_AG, INPUT);
  pinMode(IR_AD, INPUT);

  resetStationIR();
  dernierSignalStationMs = millis();

  Serial.println("IR pret : AG GPIO39 / AD GPIO21");
}

void LireIR() {
  if (modeRobot != MODE_RETOUR_BASE) {
    resetStationIR();
    return;
  }

  StationSignal type = lireDirectionIR();

  if (type != STATION_AUCUN) {
    confirmerSignalStation(type);
    dernierSignalStationMs = millis();

    Serial.print("[IR] Signal : ");

    if (type == STATION_GAUCHE) Serial.print("GAUCHE");
    else if (type == STATION_DROITE) Serial.print("DROITE");
    else if (type == STATION_CENTRE) Serial.print("CENTRE");

    Serial.print(" | Validation: ");
    Serial.print(compteurConfirmation);
    Serial.print("/");
    Serial.println(CONFIRMATIONS_NECESSAIRES);
  }

  if (millis() - dernierSignalStationMs > TIMEOUT_SIGNAL_MS) {
    resetStationIR();
  }
}