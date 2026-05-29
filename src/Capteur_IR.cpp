#include <Arduino.h>
#include <IRremote.hpp>

#include "Capteur_IR.h"
#include "WebControl.h"

// CHANGEMENT : réduit de 3 à 2 — avec des rafales périodiques, exiger 3 confirmations
// consécutives était trop strict. Le robot devait rester parfaitement aligné pendant
// 3 cycles IR d'affilée, ce qui était rarement le cas en rotation.
#define CONFIRMATIONS_NECESSAIRES 2

// CHANGEMENT : nouveau seuil — si aucun signal reçu pendant 2500ms (au lieu de 1500ms),
// on invalide. La valeur de 1500ms était trop agressive : une simple rafale manquée
// (bruit, occultation brève) causait un reset et relançait le balayage inutilement.
#define TIMEOUT_SIGNAL_MS 2500

StationSignal stationSignalConfirme = STATION_AUCUN;
StationSignal dernierTypeDetecte    = STATION_AUCUN;

int compteurConfirmation        = 0;
unsigned long dernierSignalStationMs = 0;

// =====================
// OUTILS
// =====================

void resetStationIR() {
  stationSignalConfirme = STATION_AUCUN;
  dernierTypeDetecte    = STATION_AUCUN;
  compteurConfirmation  = 0;
}

void confirmerSignalStation(bool signalVu) {
  if (!signalVu) {
    return;
  }

  compteurConfirmation++;

  if (compteurConfirmation >= CONFIRMATIONS_NECESSAIRES) {
    stationSignalConfirme    = STATION_VU;
    dernierSignalStationMs   = millis();
  }
}

// =====================
// INIT
// =====================

void InitIR() {
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  resetStationIR();
  dernierSignalStationMs = millis();
  Serial.println("IR pret GPIO39 seulement");
}

// =====================
// DEBUG RAW
// =====================

IRRawDebug getIRRawDebug() {
  IRRawDebug d;
  d.rawlen   = IrReceiver.irparams.rawlen;
  d.t1       = d.rawlen > 1 ? IrReceiver.irparams.rawbuf[1] * MICROS_PER_TICK : 0;
  d.t2       = d.rawlen > 2 ? IrReceiver.irparams.rawbuf[2] * MICROS_PER_TICK : 0;
  d.t3       = d.rawlen > 3 ? IrReceiver.irparams.rawbuf[3] * MICROS_PER_TICK : 0;
  d.total    = 0;
  d.nbBursts = 0;
  for (int i = 1; i < d.rawlen; i++) {
    d.total += IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;
    if (i % 2 == 1) d.nbBursts++;
  }
  return d;
}

// =====================
// LECTURE
// =====================

void LireIR() {
  // 1. Si aucun paquet reçu
  if (!IrReceiver.decode()) {
    if (modeRobot != MODE_RETOUR_BASE) {
      resetStationIR();
      return;
    }

    // CHANGEMENT : timeout augmenté à 2500ms via la constante nommée
    if (millis() - dernierSignalStationMs > TIMEOUT_SIGNAL_MS) {
      resetStationIR();
    }
    return;
  }

  // 2. Sécurité hors mode retour base
  if (modeRobot != MODE_RETOUR_BASE) {
    resetStationIR();
    IrReceiver.resume();
    return;
  }

  // 3. Analyse du signal
  int burstsDetectes = IrReceiver.irparams.rawlen / 2;

  if (IrReceiver.irparams.rawlen >= 25 && burstsDetectes >= 12) {
    confirmerSignalStation(true);
    dernierSignalStationMs = millis();

    Serial.print("[IR] -> Signal valide ! Rawlen: ");
    Serial.print(IrReceiver.irparams.rawlen);
    Serial.print(" | Bursts: ");
    Serial.print(burstsDetectes);
    Serial.print(" | Validation: ");
    Serial.print(compteurConfirmation);
    Serial.print("/");
    Serial.println(CONFIRMATIONS_NECESSAIRES);
  }
  else {
    Serial.print("[IR] -> Signal parasite ignoré (Rawlen: ");
    Serial.print(IrReceiver.irparams.rawlen);
    Serial.println(")");
  }

  // 4. Libération du récepteur
  IrReceiver.resume();
}