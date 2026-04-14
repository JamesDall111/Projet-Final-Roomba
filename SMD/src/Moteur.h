/**************************************************************************************************
Nom du fichier : main.c
Auteur : James Dallaire
Date de création : 14 Avril 2026

Description : Contrôle d'un robot à l'aide d'une télécommande infrarouge et de capteurs de distance VL53L0X. 
              Le robot peut avancer, reculer, tourner à gauche et à droite en fonction des commandes reçues. 
              Les distances mesurées par les capteurs sont affichées sur le moniteur série.
  
***************************************************************************************************/
#ifndef MOTEUR_H
#define MOTEUR_H

#include <Arduino.h>

//  MOTEUR enable A
extern const int PIN_ENA;
extern const int PIN_IN1;
extern const int PIN_IN2;

//parametre MOTEUR PWM
extern const int PWM_FREQ;
extern const int PWM_RESOLUTION;
extern const int PWM_CHANNEL;
extern const int VITESSE_MOTEUR;

//MOTEUR avec H bridge (breadboard)
void vitesse(int v);   //protection contre les valeurs invalides
void stopMoteur();
void avancer();
void reculer();
void droite();
void gauche();
void InitMoteur();

#endif