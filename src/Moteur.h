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

enum MoveCmd {
  CMD_STOP,
  CMD_AVANCE,
  CMD_RECULE,
  CMD_GAUCHE,
  CMD_DROITE
};

extern MoveCmd moveCmd;

void initMoteurs();
void avancer();
void reculer();
void tournerGauche();
void tournerDroite();
void stopRoues();
void appliquerMouvement();

#endif