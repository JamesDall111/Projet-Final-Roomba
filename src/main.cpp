#include <Arduino.h>

#include "Moteur.h"
#include "Accessoires.h"
#include "Distance.h"
#include "Batterie.h"
#include "WebControl.h"
#include "Capteur_IR.h"
#include "Gestion_SansFil.h"

// =====================
// VITESSES
// =====================
const int VITESSE_AUTO_AVANCE = 145;
const int VITESSE_AUTO_TOURNE = 95;
const int VITESSE_AUTO_RECUL  = 120;

const int VITESSE_BASE_AVANCE = 80;
const int VITESSE_BASE_TOURNE = 75;

// =====================
// SEUILS DISTANCE
// =====================
const int DANGER_DEVANT = 120;
const int MUR_COTE      = 90;
const int STOP_BASE     = 80;

// =====================
// OUTILS MOTEURS
// =====================
void rouesStop() {
  stopRoues();
}

void avancerControle(int vitesse) {
  setRoueGauche(true, vitesse);
  setRoueDroite(true, vitesse);
}

void reculerControle(int vitesse) {
  setRoueGauche(false, vitesse);
  setRoueDroite(false, vitesse);
}

void tournerGaucheControle(int vitesse) {
  setRoueGauche(false, vitesse);
  setRoueDroite(true, vitesse);
}

void tournerDroiteControle(int vitesse) {
  setRoueGauche(true, vitesse);
  setRoueDroite(false, vitesse);
}

// =====================
// MODE AUTO
// suit mur 30 sec -> sort vers milieu -> avance droit
// =====================
void updateAuto() {
  accessoiresOn();

  static unsigned long debutCycleMur = millis();
  static unsigned long debutPhase = 0;
  static int phaseAuto = 0;
  static bool alterneCote = false;

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);
  int coteG  = lireCapteur(2);
  int coteD  = lireCapteur(3);

  int avantMin = min(avantG, avantD);

  // Obstacle devant
  if (avantMin < DANGER_DEVANT) {
    reculerControle(VITESSE_AUTO_RECUL);
    delay(180);

    if (alterneCote) {
      tournerGaucheControle(VITESSE_AUTO_RECUL);
    } else {
      tournerDroiteControle(VITESSE_AUTO_RECUL);
    }

    delay(350);
    rouesStop();
    return;
  }

  // Phase 0 : suivi mur pendant 30 secondes
  if (phaseAuto == 0) {

    if (millis() - debutCycleMur >= 30000) {
      phaseAuto = 1;
      debutPhase = millis();
      alterneCote = !alterneCote;
      Serial.println("[AUTO] Sortie du mur");
      return;
    }

    if (coteG < MUR_COTE) {
      tournerDroiteControle(VITESSE_AUTO_TOURNE);
    }
    else if (coteD < MUR_COTE) {
      tournerGaucheControle(VITESSE_AUTO_TOURNE);
    }
    else {
      avancer();
    }

    return;
  }

  // Phase 1 : tourner pour quitter le mur
  if (phaseAuto == 1) {
    if (alterneCote) {
      tournerDroiteControle(120);
    } else {
      tournerGaucheControle(120);
    }

    if (millis() - debutPhase >= 650) {
      phaseAuto = 2;
      debutPhase = millis();
      Serial.println("[AUTO] Avance vers milieu");
    }

    return;
  }

  // Phase 2 : avancer droit vers le milieu
  if (phaseAuto == 2) {
    avancerControle(VITESSE_AUTO_AVANCE);

    if (millis() - debutPhase >= 2500) {
      phaseAuto = 0;
      debutCycleMur = millis();
      Serial.println("[AUTO] Reprise suivi mur");
    }

    return;
  }
}
// =====================
// VARIABLES RETOUR BASE
// =====================
unsigned long chronoBalayageBase = 0;
const unsigned long INTERVALLE_BALAYAGE = 2000;

bool enPhaseBalayage = false;
unsigned long debutPhaseBalayage = 0;

unsigned long chronoCorrectionBase = 0;
const unsigned long INTERVALLE_CORRECTION_BASE = 800;

bool pauseCorrectionBase = false;
unsigned long debutPauseCorrectionBase = 0;
// =====================
// MODE RETOUR BASE
// GPIO39 seulement
// =====================
void updateRetourBase() {
  accessoiresOff();
  unsigned long tempsActuel = millis();

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);

  const int VITESSE_BASE_TOURNE = 65;
  const int VITESSE_BASE_AVANCE_G = 80;
  const int VITESSE_BASE_AVANCE_D = 115;

  // --- 1. ARRÊT SUR CONTACTS OU PROXIMITÉ ---
  if (avantG < 80 || avantD < 80 || donneesStation.contactsAlimentes) {
    Serial.println("[Base] -> Connexion/proximité. Arrêt.");
    modeRobot = MODE_MANUEL;
    moveCmd = CMD_STOP;
    appliquerMouvement();
    resetStationIR();
    enPhaseBalayage = false;
    pauseCorrectionBase = false;
    return;
  }

  // --- 2. PAUSE DE RECALAGE ---
  if (pauseCorrectionBase) {
    stopRoues();

    if (tempsActuel - debutPauseCorrectionBase >= 250) {
      pauseCorrectionBase = false;

      if (stationSignalConfirme == STATION_VU) {
        Serial.println("[Base] -> Signal encore vu, trajectoire OK.");
        chronoCorrectionBase = tempsActuel;
      } else {
        Serial.println("[Base] -> Signal perdu, correction.");
        enPhaseBalayage = true;
        debutPhaseBalayage = tempsActuel;
      }
    }

    return;
  }

  // --- 3. SI LA BASE EST VUE : AVANCE PAR PETITS COUPS ---
  if (stationSignalConfirme == STATION_VU) {

    if (tempsActuel - chronoCorrectionBase >= INTERVALLE_CORRECTION_BASE) {
      pauseCorrectionBase = true;
      debutPauseCorrectionBase = tempsActuel;
      stopRoues();
      return;
    }

    setRoueGauche(true, VITESSE_BASE_AVANCE_G);
    setRoueDroite(true, VITESSE_BASE_AVANCE_D);

    enPhaseBalayage = false;
    return;
  }

  // --- 4. BALAYAGE LENT POUR RETROUVER LA BASE ---
  if (enPhaseBalayage) {
    unsigned long tempsPasseBalayage = tempsActuel - debutPhaseBalayage;

    if (tempsPasseBalayage < 700) {
      setRoueGauche(false, VITESSE_BASE_TOURNE);
      setRoueDroite(true, VITESSE_BASE_TOURNE);
    }
    else if (tempsPasseBalayage < 1400) {
      setRoueGauche(true, VITESSE_BASE_TOURNE);
      setRoueDroite(false, VITESSE_BASE_TOURNE);
    }
    else if (tempsPasseBalayage < 1700) {
      stopRoues();
    }
    else {
      enPhaseBalayage = false;
      chronoBalayageBase = tempsActuel;
      Serial.println("[Base] -> Scan fini.");
    }

    return;
  }

  // --- 5. DÉCLENCHEMENT DU BALAYAGE ---
  if (tempsActuel - chronoBalayageBase >= INTERVALLE_BALAYAGE) {
    Serial.println("[Base] -> Début balayage lent.");
    enPhaseBalayage = true;
    debutPhaseBalayage = tempsActuel;
    stopRoues();
    return;
  }

  // --- 6. RECHERCHE LENTE PAR DÉFAUT ---
  setRoueGauche(true, VITESSE_BASE_TOURNE);
  setRoueDroite(false, VITESSE_BASE_TOURNE);
}
// =====================
// SETUP
// =====================
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n=== ROBOT SMD ===");

  initMoteurs();
  initAccessoires();
  initBatterie();
  initCapteurs();

  initWebControl();
  InitIR();
  initGestionSansFil();

  chronoBalayageBase = millis();
  chronoCorrectionBase = millis();

  accessoiresOff();
  rouesStop();
}

// =====================
// LOOP
// =====================
unsigned long chronoEnvoiReseau = 0;

void loop() {
  updateWebControl();
  LireIR();

  if (millis() - chronoEnvoiReseau >= 1000) {
    chronoEnvoiReseau = millis();
    updateGestionSansFil();
  }

  if (modeRobot == MODE_AUTO) {
    updateAuto();
  }
  else if (modeRobot == MODE_RETOUR_BASE) {
    updateRetourBase();
  }
  else {
   
  }
}