#include <Arduino.h>

#include "Telecommande_IR.h"
#include "Moteur.h"
#include "Accessoires.h"
#include "Distance.h"
#include "Batterie.h"
#include "WebControl.h"
#include "Capteur_IR.h"

// =====================
// AUTO
// =====================
const int DANGER_DEVANT = 120;
const int MUR_DEVANT = 190;
const int COTE_DANGER = 70;

const unsigned long TEMPS_RECUL = 160;
const unsigned long TEMPS_TOURNE_GAUCHE = 220;
const unsigned long TEMPS_TOURNE_DROITE = 170;
const unsigned long TEMPS_PAUSE = 80;

enum AutoState {
  AUTO_ANALYSE,
  AUTO_RECULE,
  AUTO_TOURNE_G,
  AUTO_TOURNE_D,
  AUTO_PAUSE
};

AutoState autoState = AUTO_ANALYSE;

unsigned long autoFinAction = 0;
bool dernierTourneDroite = true;

// =====================
// OUTILS AUTO
// =====================
void setAutoState(AutoState state, unsigned long duree) {
  autoState = state;
  autoFinAction = millis() + duree;
}

bool tempsFini() {
  return millis() >= autoFinAction;
}

void resetAuto() {
  autoState = AUTO_ANALYSE;
  autoFinAction = 0;
}

// =====================
// MODE AUTOMATIQUE
// =====================
void updateAuto() {
  accessoiresOn();

  if (autoState == AUTO_RECULE) {
    moveCmd = CMD_RECULE;
    appliquerMouvement();

    if (tempsFini()) {
      if (dernierTourneDroite) {
        dernierTourneDroite = false;
        moveCmd = CMD_GAUCHE;
        setAutoState(AUTO_TOURNE_G, TEMPS_TOURNE_GAUCHE);
      } else {
        dernierTourneDroite = true;
        moveCmd = CMD_DROITE;
        setAutoState(AUTO_TOURNE_D, TEMPS_TOURNE_DROITE);
      }
    }
    return;
  }

  if (autoState == AUTO_TOURNE_G || autoState == AUTO_TOURNE_D) {
    appliquerMouvement();

    if (tempsFini()) {
      moveCmd = CMD_STOP;
      appliquerMouvement();
      setAutoState(AUTO_PAUSE, TEMPS_PAUSE);
    }
    return;
  }

  if (autoState == AUTO_PAUSE) {
    moveCmd = CMD_STOP;
    appliquerMouvement();

    if (tempsFini()) {
      resetAuto();
    }
    return;
  }

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);
  int coteG  = lireCapteur(2);
  int coteD  = lireCapteur(3);

  int avantMin = min(avantG, avantD);

  if (avantMin < DANGER_DEVANT) {
    moveCmd = CMD_STOP;
    appliquerMouvement();
    setAutoState(AUTO_RECULE, TEMPS_RECUL);
    return;
  }

  if (avantMin < MUR_DEVANT) {
    moveCmd = CMD_STOP;
    appliquerMouvement();

    if (coteG > coteD + 80) {
      moveCmd = CMD_GAUCHE;
      setAutoState(AUTO_TOURNE_G, TEMPS_TOURNE_GAUCHE);
    } else if (coteD > coteG + 80) {
      moveCmd = CMD_DROITE;
      setAutoState(AUTO_TOURNE_D, TEMPS_TOURNE_DROITE);
    } else {
      if (dernierTourneDroite) {
        dernierTourneDroite = false;
        moveCmd = CMD_GAUCHE;
        setAutoState(AUTO_TOURNE_G, TEMPS_TOURNE_GAUCHE);
      } else {
        dernierTourneDroite = true;
        moveCmd = CMD_DROITE;
        setAutoState(AUTO_TOURNE_D, TEMPS_TOURNE_DROITE);
      }
    }
    return;
  }

  if (coteG < COTE_DANGER) {
    moveCmd = CMD_DROITE;
    setAutoState(AUTO_TOURNE_D, 90);
    return;
  }

  if (coteD < COTE_DANGER) {
    moveCmd = CMD_GAUCHE;
    setAutoState(AUTO_TOURNE_G, 110);
    return;
  }

  moveCmd = CMD_AVANCE;
  appliquerMouvement();
}

// =====================
// RETOUR BASE
// =====================
void updateRetourBase() {
  accessoiresOff();

  static unsigned long dernierUpdate = 0;

  if (millis() - dernierUpdate < 150) {
    return;
  }

  dernierUpdate = millis();

  if (stationSignalConfirme == STATION_VU) {
    Serial.println("RETOUR BASE : VU -> AVANCER");

    moveCmd = CMD_AVANCE;
    appliquerMouvement();
    delay(300);

    moveCmd = CMD_STOP;
    appliquerMouvement();
    return;
  }

  // Pas de signal : tourne pour chercher
  Serial.println("RETOUR BASE : AUCUN -> CHERCHER");

  moveCmd = CMD_DROITE;
  appliquerMouvement();
  delay(80);

  moveCmd = CMD_STOP;
  appliquerMouvement();
}

// =====================
// SETUP
// =====================
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("=== ROBOT SMD ===");

  initMoteurs();
  initAccessoires();
  initBatterie();
  initCapteurs();
  initWebControl();
  InitIR();

  accessoiresOff();
  moveCmd = CMD_STOP;
  appliquerMouvement();

  Serial.println("Robot pret");
}

// =====================
// LOOP
// =====================
void loop() {
  updateWebControl();
  LireIR();

  if (modeRobot == MODE_AUTO) {
    updateAuto();
  }
  else if (modeRobot == MODE_RETOUR_BASE) {
    updateRetourBase();
  }
  else if (modeRobot == MODE_MANUEL) {
    // Rien : robot immobile
  }
}