#include <Arduino.h>
#include <IRremote.hpp>

#include "Capteur_IR.h"
#include "WebControl.h"

#define CONFIRMATIONS_NECESSAIRES 3

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
  // 1. Si aucun paquet de données n'est reçu par le composant matériel
  if (!IrReceiver.decode()) {
    if (modeRobot != MODE_RETOUR_BASE) {
      resetStationIR();
      return;
    }

    // En mode retour base, si on ne reçoit aucun signal valide pendant plus de 1500 ms
    if (millis() - dernierSignalStationMs > 1500) {
      resetStationIR();
    }
    return;
  }

  // 2. Sécurité : si on n'est pas en mode retour base, on ignore tout traitement
  if (modeRobot != MODE_RETOUR_BASE) {
    resetStationIR();
    IrReceiver.resume(); // Libère le tampon
    return;
  }

  // 3. ANALYSE STATISTIQUE DU SIGNAL (Basé sur tes valeurs réelles : rawlen 60, bursts 30)
  int burstsDetectes = IrReceiver.irparams.rawlen / 2;

  if (IrReceiver.irparams.rawlen >= 25 && burstsDetectes >= 12) {
    // Le signal coche toutes les cases de ta balise
    confirmerSignalStation(true);
    
    // On met à jour le garde-temps du dernier signal reçu
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
    // Signal trop court (bruit ambiant ou parasite)
    Serial.print("[IR] -> Signal parasite ignoré (Rawlen: ");
    Serial.print(IrReceiver.irparams.rawlen);
    Serial.println(")");
  }

  // 4. Libération obligatoire du récepteur pour la prochaine salve
  IrReceiver.resume();
}