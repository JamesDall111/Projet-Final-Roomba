#include <Arduino.h>

#include "Moteur.h"
#include "Accessoires.h"
#include "Distance.h"
#include "Batterie.h"
#include "WebControl.h"

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

void setAutoState(AutoState state, unsigned long duree) {
  autoState = state;
  autoFinAction = millis() + duree;
}

bool tempsFini() {
  return millis() >= autoFinAction;
}

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
      autoState = AUTO_ANALYSE;
    }
    return;
  }

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);
  int coteG  = lireCapteur(2);
  int coteD  = lireCapteur(3);

  int avantMin = min(avantG, avantD);

  // Tres proche devant : recule un peu
  if (avantMin < DANGER_DEVANT) {
    moveCmd = CMD_STOP;
    appliquerMouvement();
    setAutoState(AUTO_RECULE, TEMPS_RECUL);
    return;
  }

  // Mur devant : tourne vers le cote le plus libre
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

  // Si vraiment trop proche d'un cote, petite correction
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

  // Sinon avance
  moveCmd = CMD_AVANCE;
  appliquerMouvement();
}

void setup() {
  Serial.begin(115200);

  initMoteurs();
  initAccessoires();
  initBatterie();
  initCapteurs();
  initWebControl();
}

void loop() {
  updateWebControl();

  if (modeRobot == MODE_AUTO) {
    updateAuto();
  }
}