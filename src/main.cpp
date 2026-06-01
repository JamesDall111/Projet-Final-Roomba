#include <Arduino.h>

#include "Moteur.h"
#include "Accessoires.h"
#include "Distance.h"
#include "Batterie.h"
#include "WebControl.h"
#include "Capteur_IR.h"
#include "Gestion_SansFil.h"

// =====================
// SEUILS
// =====================
const int DANGER_DEVANT = 120;
const int MUR_COTE = 90;
const int STOP_BASE = 80;

// =====================
// VITESSES
// =====================
const int VITESSE_TOURNE_AUTO = 110;
const int VITESSE_TOURNE_BASE = 65;
const int VITESSE_RECUL = 120;

// =====================
// OUTILS MOTEURS
// =====================
void tournerGaucheControle(int vitesse) {
  setRoueGauche(false, vitesse);
  setRoueDroite(true, vitesse);
}

void tournerDroiteControle(int vitesse) {
  setRoueGauche(true, vitesse);
  setRoueDroite(false, vitesse);
}

void reculerControle(int vitesse) {
  setRoueGauche(false, vitesse);
  setRoueDroite(false, vitesse);
}

// =====================
// MODE AUTO
// 30 sec suivi mur -> tourne -> avance droit -> recommence
// =====================
void updateAuto() {
  accessoiresOn();

  static int phaseAuto = 0;
  static unsigned long debutPhase = millis();
  static bool coteSortieDroite = true;

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);
  int coteG  = lireCapteur(2);
  int coteD  = lireCapteur(3);

  int avantMin = min(avantG, avantD);

  // Sécurité obstacle devant
  if (avantMin < DANGER_DEVANT) {
    reculerControle(VITESSE_RECUL);
    delay(180);

    if (coteSortieDroite) tournerDroiteControle(VITESSE_TOURNE_AUTO);
    else tournerGaucheControle(VITESSE_TOURNE_AUTO);

    delay(350);
    stopRoues();
    return;
  }

  // Phase 0 : suivre les murs pendant 30 sec
  if (phaseAuto == 0) {
    if (millis() - debutPhase >= 30000) {
      phaseAuto = 1;
      debutPhase = millis();
      coteSortieDroite = !coteSortieDroite;
      Serial.println("[AUTO] Sortie du mur");
      return;
    }

    if (coteG < MUR_COTE) {
      tournerDroiteControle(90);
    }
    else if (coteD < MUR_COTE) {
      tournerGaucheControle(90);
    }
    else {
      avancer();   // utilise ta calibration 90 / 125
    }

    return;
  }

  // Phase 1 : tourner pour quitter le mur
  if (phaseAuto == 1) {
    if (coteSortieDroite) tournerDroiteControle(110);
    else tournerGaucheControle(110);

    if (millis() - debutPhase >= 650) {
      phaseAuto = 2;
      debutPhase = millis();
      Serial.println("[AUTO] Avance tout droit");
    }

    return;
  }

  // Phase 2 : avancer droit vers le milieu
  if (phaseAuto == 2) {
    avancer();   // utilise ta calibration 90 / 125

    if (millis() - debutPhase >= 3000) {
      phaseAuto = 0;
      debutPhase = millis();
      Serial.println("[AUTO] Reprise suivi mur");
    }

    return;
  }
}

void updateRetourBase() {

  accessoiresOff();

  static int phaseBase = 0;
  static unsigned long debutPhase = 0;

  static StationSignal derniereDirection = STATION_AUCUN;
  static StationSignal dernierSignalIR = STATION_AUCUN;
  static unsigned long tempsDernierSignalIR = 0;

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);
  int avantMin = min(avantG, avantD);

  bool centreProbable = false;

  // ===== DÉTECTION CENTRE PROBABLE =====
  // Si ça alterne gauche/droite rapidement, le robot est probablement bien aligné
  if (stationSignalConfirme != STATION_AUCUN) {

    if ((dernierSignalIR == STATION_GAUCHE && stationSignalConfirme == STATION_DROITE) ||
        (dernierSignalIR == STATION_DROITE && stationSignalConfirme == STATION_GAUCHE)) {

      if (millis() - tempsDernierSignalIR < 500) {
        centreProbable = true;
      }
    }

    dernierSignalIR = stationSignalConfirme;
    tempsDernierSignalIR = millis();
  }

  // ===== PUSH FINAL =====
  if (phaseBase == 4) {
    avancer();

    if (millis() - debutPhase > 350) {
      Serial.println("[BASE] Arrive station");
      stopRoues();
      modeRobot = MODE_MANUEL;
      resetStationIR();

      phaseBase = 0;
      derniereDirection = STATION_AUCUN;
      dernierSignalIR = STATION_AUCUN;
      centreProbable = false;
    }
    return;
  }

  // ===== PROCHE BASE =====
  if (stationSignalConfirme != STATION_AUCUN && avantMin < STOP_BASE) {
    Serial.println("[BASE] Proche -> push final");
    stopRoues();
    debutPhase = millis();
    phaseBase = 4;
    return;
  }

  // ===== MÉMORISE DERNIÈRE DIRECTION =====
  if (stationSignalConfirme == STATION_GAUCHE ||
      stationSignalConfirme == STATION_DROITE ||
      stationSignalConfirme == STATION_CENTRE) {
    derniereDirection = stationSignalConfirme;
  }

  // ===== PHASE 0 : DÉCISION =====
  if (phaseBase == 0) {

    if (stationSignalConfirme == STATION_CENTRE || centreProbable) {
      Serial.println("[BASE] Centre/probable -> avance");
      resetStationIR();
      debutPhase = millis();
      phaseBase = 2;
      return;
    }

    if (stationSignalConfirme == STATION_GAUCHE) {
      Serial.println("[BASE] Signal gauche -> corrige gauche");
      resetStationIR();
      debutPhase = millis();
      phaseBase = 10;
      return;
    }

    if (stationSignalConfirme == STATION_DROITE) {
      Serial.println("[BASE] Signal droite -> corrige droite");
      resetStationIR();
      debutPhase = millis();
      phaseBase = 11;
      return;
    }

    if (derniereDirection == STATION_GAUCHE) {
      Serial.println("[BASE] Perdu apres gauche -> corrige gauche");
      debutPhase = millis();
      phaseBase = 10;
      return;
    }

    if (derniereDirection == STATION_DROITE) {
      Serial.println("[BASE] Perdu apres droite -> corrige droite");
      debutPhase = millis();
      phaseBase = 11;
      return;
    }

    Serial.println("[BASE] Jamais vu -> scan droite court");
    debutPhase = millis();
    phaseBase = 1;
    return;
  }

  // ===== PHASE 1 : RECHERCHE INITIALE COURTE =====
  if (phaseBase == 1) {
    tournerDroiteLent();

    if (stationSignalConfirme != STATION_AUCUN || millis() - debutPhase > 160) {
      stopRoues();
      debutPhase = millis();
      phaseBase = 3;
    }
    return;
  }

  // ===== PHASE 2 : AVANCE PLUS LONGUE =====
  if (phaseBase == 2) {
    avancer();

    if (millis() - debutPhase > 650) {
      stopRoues();
      debutPhase = millis();
      phaseBase = 3;
    }
    return;
  }

  // ===== PHASE 10 : CORRECTION GAUCHE COURTE =====
  if (phaseBase == 10) {
    tournerGaucheLent();

    if (millis() - debutPhase > 80 || stationSignalConfirme == STATION_CENTRE) {
      stopRoues();
      debutPhase = millis();
      phaseBase = 3;
    }
    return;
  }

  // ===== PHASE 11 : CORRECTION DROITE COURTE =====
  if (phaseBase == 11) {
    tournerDroiteLent();

    if (millis() - debutPhase > 80 || stationSignalConfirme == STATION_CENTRE) {
      stopRoues();
      debutPhase = millis();
      phaseBase = 3;
    }
    return;
  }

  // ===== PHASE 3 : PAUSE ÉCOUTE =====
  if (phaseBase == 3) {
    stopRoues();

    if (millis() - debutPhase > 220) {
      phaseBase = 0;
    }
    return;
  }
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

  accessoiresOff();
  stopRoues();
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
    // MODE_MANUEL : ne pas arrêter ici
  }
}