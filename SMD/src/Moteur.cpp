/**************************************************************************************************
Nom du fichier : Moteur.cpp
Auteur : James Dallaire
Date de création : 14 Avril 2026

Description : Gestion du moteur du robot avec un H-bridge.
              Ce module permet de contrôler la vitesse et le sens de rotation
              pour avancer, reculer, tourner à gauche et à droite.

***************************************************************************************************/
#include "Moteur.h"

//  MOTEUR enable A

const int PIN_ENA = 14;//changer les pins 
const int PIN_IN1 = 32;
const int PIN_IN2 = 33;

/*                      //changer les pins dependant du 2me moteurs
const int PIN_ENA = ??;
const int PIN_IN3 = ??;
const int PIN_IN4 = ??;
*/

//parametre MOTEUR PWM
const int PWM_FREQ = 1000;
const int PWM_RESOLUTION = 8;
const int PWM_CHANNEL = 0;
const int VITESSE_MOTEUR = 200;

//MOTEUR avec H bridge (breadboard)
void vitesse(int v)   //protection contre les valeurs invalides
{
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  ledcWrite(PWM_CHANNEL, v);
}

void stopMoteur() 
{
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  vitesse(0);
  Serial.println("STOP");
}

void avancer() 
{
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  vitesse(VITESSE_MOTEUR);
  Serial.println("AVANCE");
}

void reculer() 
{
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  vitesse(VITESSE_MOTEUR);
  Serial.println("RECULE");
}

void droite()                       //modifer pour la droite et la gauche 
{
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  vitesse(VITESSE_MOTEUR);
  Serial.println("DROITE");
}

void gauche() 
{
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  vitesse(VITESSE_MOTEUR);
  Serial.println("GAUCHE");
}

void InitMoteur()           //pour un seul moteur doit en rajouter un de plus********
{
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_ENA, PWM_CHANNEL);

  stopMoteur();
}