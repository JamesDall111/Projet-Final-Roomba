#include <Arduino.h>

#include "Moteur.h"
#include "Accessoires.h"
#include "Distance.h"
#include "Batterie.h"
#include "WebControl.h"
#include "Capteur_IR.h"
#include "Gestion_SansFil.h" 

// =====================
// SEUILS & CONSTANTES AUTO
// =====================
const int DANGER_DEVANT = 120;
const int MUR_DEVANT    = 190;
const int COTE_DANGER   = 70;

const unsigned long TEMPS_RECUL         = 160;
const unsigned long TEMPS_TOURNE_GAUCHE = 220;
const unsigned long TEMPS_TOURNE_DROITE = 170;
const unsigned long TEMPS_PAUSE         = 80;

enum AutoState { AUTO_ANALYSE, AUTO_RECULE, AUTO_TOURNE_G, AUTO_TOURNE_D, AUTO_PAUSE };
AutoState autoState    = AUTO_ANALYSE;
unsigned long autoFinAction = 0;
bool dernierTourneDroite   = true;

// =====================
// OUTILS MODE AUTO
// =====================
void setAutoState(AutoState state, unsigned long duree) {
  autoState    = state;
  autoFinAction = millis() + duree;
}

bool tempsFini() { return millis() >= autoFinAction; }
void resetAuto() { autoState = AUTO_ANALYSE; autoFinAction = 0; }

// =====================
// COMPORTEMENT AUTOMATIQUE
// =====================
void updateAuto() {
  accessoiresOn();
  
  if (autoState == AUTO_RECULE) {
    moveCmd = CMD_RECULE; appliquerMouvement();
    if (tempsFini()) {
      if (dernierTourneDroite) {
        dernierTourneDroite = false; moveCmd = CMD_GAUCHE;
        setAutoState(AUTO_TOURNE_G, TEMPS_TOURNE_GAUCHE);
      } else {
        dernierTourneDroite = true; moveCmd = CMD_DROITE;
        setAutoState(AUTO_TOURNE_D, TEMPS_TOURNE_DROITE);
      }
    }
    return;
  }
  
  if (autoState == AUTO_TOURNE_G || autoState == AUTO_TOURNE_D) {
    appliquerMouvement();
    if (tempsFini()) {
      moveCmd = CMD_STOP; appliquerMouvement();
      setAutoState(AUTO_PAUSE, TEMPS_PAUSE);
    }
    return;
  }
  
  if (autoState == AUTO_PAUSE) {
    moveCmd = CMD_STOP; appliquerMouvement();
    if (tempsFini()) { resetAuto(); }
    return;
  }

  int avantG = lireCapteur(0); int avantD = lireCapteur(1);
  int coteG  = lireCapteur(2);  int coteD  = lireCapteur(3);
  int avantMin = min(avantG, avantD);

  if (avantMin < DANGER_DEVANT) {
    moveCmd = CMD_STOP; appliquerMouvement();
    setAutoState(AUTO_RECULE, TEMPS_RECUL); return;
  }
  
  if (avantMin < MUR_DEVANT) {
    moveCmd = CMD_STOP; appliquerMouvement();
    if (coteG > coteD + 80) {
      moveCmd = CMD_GAUCHE; setAutoState(AUTO_TOURNE_G, TEMPS_TOURNE_GAUCHE);
    } else if (coteD > coteG + 80) {
      moveCmd = CMD_DROITE; setAutoState(AUTO_TOURNE_D, TEMPS_TOURNE_DROITE);
    } else {
      if (dernierTourneDroite) {
        dernierTourneDroite = false; moveCmd = CMD_GAUCHE;
        setAutoState(AUTO_TOURNE_G, TEMPS_TOURNE_GAUCHE);
      } else {
        dernierTourneDroite = true; moveCmd = CMD_DROITE;
        setAutoState(AUTO_TOURNE_D, TEMPS_TOURNE_DROITE);
      }
    }
    return;
  }
  if (coteG < COTE_DANGER) { moveCmd = CMD_DROITE; setAutoState(AUTO_TOURNE_D, 90); return; }
  if (coteD < COTE_DANGER) { moveCmd = CMD_GAUCHE; setAutoState(AUTO_TOURNE_G, 110); return; }

  moveCmd = CMD_AVANCE;
  appliquerMouvement();
}

// =====================
// VARIABLES RETOUR BASE
// =====================
unsigned long chronoBalayageBase = 0;       
const unsigned long INTERVALLE_BALAYAGE = 2000; 
bool enPhaseBalayage = false;              
unsigned long debutPhaseBalayage = 0;      

// =====================
// COMPORTEMENT RETOUR BASE
// =====================
void updateRetourBase() {
  accessoiresOff();
  unsigned long tempsActuel = millis();

  int avantG = lireCapteur(0);
  int avantD = lireCapteur(1);
  
  // --- 1. ARRÊT SUR CONTACTS OU PROXIMITÉ DE LA BASE ---
  if (avantG < 80 || avantD < 80 || donneesStation.contactsAlimentes) { 
    Serial.println("[Base] -> Connexion établie ou obstacle proche. Verrouillage !");
    modeRobot = MODE_MANUEL; 
    moveCmd = CMD_STOP;
    appliquerMouvement();
    resetStationIR();
    enPhaseBalayage = false;
    return;
  }

  // --- 2. LOGIQUE DE BALAYAGE SÉQUENTIEL (CADENCÉ) ---
  if (enPhaseBalayage) {
    unsigned long tempsPasseBalayage = tempsActuel - debutPhaseBalayage;
    if (tempsPasseBalayage < 150)       moveCmd = CMD_GAUCHE;
    else if (tempsPasseBalayage < 350)  moveCmd = CMD_DROITE;
    else if (tempsPasseBalayage < 550)  moveCmd = CMD_STOP; // Pause utile pour stabiliser l'IR
    else {
      enPhaseBalayage = false;
      chronoBalayageBase = tempsActuel; 
      Serial.println("[Base] -> Scan fini, reprise de la marche cadencée.");
    }
    appliquerMouvement();
    return; 
  }

  // --- 3. DÉCLENCHEMENT DU BALAYAGE TOUTES LES 2 SECONDES ---
  if (tempsActuel - chronoBalayageBase >= INTERVALLE_BALAYAGE) {
    enPhaseBalayage = true;
    debutPhaseBalayage = tempsActuel;
    moveCmd = CMD_STOP; 
    appliquerMouvement();
    return;
  }

  // --- 4. NAVIGATION LENTE VERS LA BALISE ---
  if (stationSignalConfirme == STATION_VU) {
    moveCmd = CMD_AVANCE; 
  } else {
    moveCmd = CMD_DROITE; 
  }
  appliquerMouvement();
}

// =====================
// INITIALISATION
// =====================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== ROBOT SMD ===");

  initMoteurs();
  initAccessoires();
  initBatterie();
  initCapteurs();
  
  initWebControl(); // 1. Allume le Wi-Fi en mode AP_STA et lance le site HTML
  InitIR();
  initGestionSansFil(); // Initialise la nouvelle configuration ESP-NOW

  accessoiresOff();
  moveCmd = CMD_STOP;
  appliquerMouvement();
  chronoBalayageBase = millis();
}

// =====================
// BOUCLE PRINCIPALE
// =====================
unsigned long chronoEnvoiReseau = 0;

void loop() {
  updateWebControl();
  LireIR();

  // Envoi asynchrone des rapports de batterie à la station toutes les 1000ms
  if (millis() - chronoEnvoiReseau >= 1000) {
    chronoEnvoiReseau = millis();
    updateGestionSansFil(); // S'occupe d'envoyer la batterie toutes les secondes
  }

  // Sélections des comportements du robot
  if (modeRobot == MODE_AUTO)            updateAuto();
  else if (modeRobot == MODE_RETOUR_BASE) updateRetourBase();
}