#include <Arduino.h>
#include <IRremote.hpp>

#include "Capteur_IR.h"
#include "Telecommande_IR.h"
#include "WebControl.h"

#define CONFIRMATIONS_NECESSAIRES 3

StationSignal stationSignalConfirme = STATION_AUCUN;
StationSignal dernierTypeDetecte = STATION_AUCUN;

int compteurConfirmation = 0;
unsigned long dernierSignalStationMs = 0;

// =====================
// OUTILS
// =====================

bool estVraiSignalNEC() {
  if (IrReceiver.irparams.rawlen < 4) {
    return false;
  }

  uint32_t t1 = IrReceiver.irparams.rawbuf[1] * MICROS_PER_TICK;
  uint32_t t2 = IrReceiver.irparams.rawbuf[2] * MICROS_PER_TICK;

  bool timingsNEC = (t1 >= 8000 && t1 <= 10000 &&
                     t2 >= 3500 && t2 <= 5500);

  if (timingsNEC && IrReceiver.irparams.rawlen >= 50) {
    return true;
  }

  return false;
}

bool estSignalStation(int rawlen, uint32_t total) {
  return (rawlen >= 50 && rawlen <= 70 &&
          total >= 40000 && total <= 90000);
}

void resetStationIR() {
  stationSignalConfirme = STATION_AUCUN;
  dernierTypeDetecte = STATION_AUCUN;
  compteurConfirmation = 0;
}

void confirmerSignalStation(bool signalVu) {
  if (!signalVu) {
    return;
  }

  compteurConfirmation++;

  if (compteurConfirmation >= CONFIRMATIONS_NECESSAIRES) {
    stationSignalConfirme = STATION_VU;
    dernierSignalStationMs = millis();
    Serial.println("Station IR : VU");
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
  if (!IrReceiver.decode()) {
    if (modeRobot != MODE_RETOUR_BASE) {
      resetStationIR();
      return;
    }

    if (millis() - dernierSignalStationMs > 2000) {
      resetStationIR();
    }

    return;
  }

  uint16_t adresse = IrReceiver.decodedIRData.address;
  uint8_t  cmd     = IrReceiver.decodedIRData.command;
  bool repetition  = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

  bool vraiNEC = estVraiSignalNEC();

  // =====================
  // TELECOMMANDE : MODE SEULEMENT
  // =====================
  if (vraiNEC && adresse == REMOTE_ADDRESS && commandeValide(cmd)) {
    gererCommandeTelecommande(cmd, repetition);
    IrReceiver.resume();
    return;
  }

  // =====================
  // STATION SEULEMENT EN RETOUR BASE
  // =====================
  if (modeRobot != MODE_RETOUR_BASE) {
    resetStationIR();
    IrReceiver.resume();
    return;
  }

  uint32_t total = 0;
  for (int i = 1; i < IrReceiver.irparams.rawlen; i++) {
    total += IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;
  }

  bool signalVu = estSignalStation(IrReceiver.irparams.rawlen, total);
  confirmerSignalStation(signalVu);

  IrReceiver.resume();
}