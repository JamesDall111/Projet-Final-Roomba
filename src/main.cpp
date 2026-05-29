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

// =====================
// MODE RETOUR BASE
// Sans RSSI, seulement LED IR sur GPIO39
// =====================
void updateRetourBase() {

  accessoiresOff();

  static int phaseBase = 0;
  static unsigned long debutPhase = 0;
  static int niveauScan = 0;

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);
  int avantMin = min(avantG, avantD);

  // ===== PUSH FINAL =====
  if (phaseBase == 4) {
    avancer();

    if (millis() - debutPhase > 350) {
      Serial.println("[BASE] Arrive station");
      stopRoues();
      modeRobot = MODE_MANUEL;
      resetStationIR();
      phaseBase = 0;
      niveauScan = 0;
    }
    return;
  }

  // ===== STOP PROCHE =====
  if (stationSignalConfirme == STATION_VU && avantMin < STOP_BASE) {
    Serial.println("[BASE] Proche -> push final");
    debutPhase = millis();
    phaseBase = 4;
    return;
  }

  // ===== PHASE 0 : SCAN DROITE PROGRESSIF =====
  if (phaseBase == 0) {

    if (stationSignalConfirme == STATION_VU) {
      Serial.println("[BASE] IR vu -> avance");
      stopRoues();
      niveauScan = 0;
      debutPhase = millis();
      phaseBase = 2;
      return;
    }

    const unsigned long dureeScan = 800;

    tournerDroiteControle(VITESSE_TOURNE_BASE);

    if (millis() - debutPhase > dureeScan) {
      stopRoues();
      debutPhase = millis();
      phaseBase = 5;
    }

    return;
  }

  // ===== PHASE 5 : PAUSE ÉCOUTE =====
  if (phaseBase == 5) {
    stopRoues();

    if (stationSignalConfirme == STATION_VU) {
      Serial.println("[BASE] IR vu pause -> avance");
      niveauScan = 0;
      debutPhase = millis();
      phaseBase = 2;
      return;
    }

    if (millis() - debutPhase > 200) {
      if (niveauScan < 4) {
        niveauScan++;
      }

      debutPhase = millis();
      phaseBase = 0;
    }

    return;
  }

  // ===== PHASE 2 : AVANCE FRANCHE =====
  if (phaseBase == 2) {
    avancer();

    if (millis() - debutPhase > 1500) {
      Serial.println("[BASE] Recalcul");
      stopRoues();
      resetStationIR();
      debutPhase = millis();
      phaseBase = 3;
    }

    return;
  }

  // ===== PHASE 3 : PAUSE AVANT RE-SCAN =====
  if (phaseBase == 3) {
    stopRoues();

    if (millis() - debutPhase > 150) {
      debutPhase = millis();
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