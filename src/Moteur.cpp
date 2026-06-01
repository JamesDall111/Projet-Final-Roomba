#include <Arduino.h>

#include "Pins.h"
#include "Moteur.h"

#define PWM_FREQ 1000
#define PWM_RES 8

#define CHANNEL_G 0
#define CHANNEL_D 1

bool INVERSE_GAUCHE = true;
bool INVERSE_DROITE = false;

int VITESSE_G_AVANCE = 90;
int VITESSE_D_AVANCE = 125;

int VITESSE_G_RECUL = 130;
int VITESSE_D_RECUL = 112;

int VITESSE_G_TOURNE = 130;
int VITESSE_D_TOURNE = 130;

int VITESSE_G_LENTE = 90;
int VITESSE_D_LENTE = 90;

MoveCmd moveCmd = CMD_STOP;

// =====================
// ROUE GAUCHE
// =====================
void setRoueGauche(bool avant, int vitesse) {
  vitesse = constrain(vitesse, 0, 255);

  if (INVERSE_GAUCHE) {
    avant = !avant;
  }

  digitalWrite(ROUE_G_DIR, avant ? HIGH : LOW);
  ledcWrite(CHANNEL_G, vitesse);
}

// =====================
// ROUE DROITE
// =====================
void setRoueDroite(bool avant, int vitesse) {
  vitesse = constrain(vitesse, 0, 255);

  if (INVERSE_DROITE) {
    avant = !avant;
  }

  digitalWrite(ROUE_D_DIR, avant ? HIGH : LOW);
  ledcWrite(CHANNEL_D, vitesse);
}

// =====================
// INIT
// =====================
void initMoteurs() {
  pinMode(ROUE_G_DIR, OUTPUT);
  pinMode(ROUE_D_DIR, OUTPUT);

  ledcSetup(CHANNEL_G, PWM_FREQ, PWM_RES);
  ledcSetup(CHANNEL_D, PWM_FREQ, PWM_RES);

  ledcAttachPin(ROUE_G_PWM, CHANNEL_G);
  ledcAttachPin(ROUE_D_PWM, CHANNEL_D);

  stopRoues();

  Serial.println("Moteurs OK");
}

// =====================
// STOP
// =====================
void stopRoues() {
  ledcWrite(CHANNEL_G, 0);
  ledcWrite(CHANNEL_D, 0);

  digitalWrite(ROUE_G_DIR, LOW);
  digitalWrite(ROUE_D_DIR, LOW);
}

// =====================
// AVANCER
// =====================
void avancer() {
  setRoueGauche(true, VITESSE_G_AVANCE);
  setRoueDroite(true, VITESSE_D_AVANCE);
}

// =====================
// RECULER
// =====================
void reculer() {
  setRoueGauche(false, VITESSE_G_RECUL);
  setRoueDroite(false, VITESSE_D_RECUL);
}

// =====================
// TOURNER GAUCHE
// =====================
void tournerGauche() {
  setRoueGauche(false, VITESSE_G_TOURNE);
  setRoueDroite(true, VITESSE_D_TOURNE);
}

// =====================
// TOURNER DROITE
// =====================
void tournerDroite() {
  setRoueGauche(true, VITESSE_G_TOURNE);
  setRoueDroite(false, VITESSE_D_TOURNE);
}

// =====================
// TOURNER LENT
// =====================
void tournerGaucheLent() {
  setRoueGauche(false, VITESSE_G_LENTE);
  setRoueDroite(true, VITESSE_D_LENTE);
}

void tournerDroiteLent() {
  setRoueGauche(true, VITESSE_G_LENTE);
  setRoueDroite(false, VITESSE_D_LENTE);
}

// =====================
// APPLIQUER
// =====================
void appliquerMouvement() {
  if (moveCmd == CMD_STOP) {
    stopRoues();
  }
  else if (moveCmd == CMD_AVANCE) {
    avancer();
  }
  else if (moveCmd == CMD_RECULE) {
    reculer();
  }
  else if (moveCmd == CMD_GAUCHE) {
    tournerGauche();
  }
  else if (moveCmd == CMD_DROITE) {
    tournerDroite();
  }
}