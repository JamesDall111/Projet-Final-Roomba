#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "Gestion_SansFil.h"
#include "Batterie.h"
#include "WebControl.h"
#include "Moteur.h"
#include "Accessoires.h"

// Adresse MAC physique de la Station
uint8_t macStation[6] = {0x90, 0xE5, 0xB1, 0xD5, 0xEF, 0x60}; 

// Allocation de la mémoire pour les variables globales
StructRobot donneesRobot;
StructStation donneesStation;
unsigned long dernierEnvoiRadioMs = 0;
int rssiStation = -100; // Stockage de la force du signal (dBm)

// Mouchard d'envoi
void callbackEnvoiRadio(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("[Robot] Envoi ESP-NOW : ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("LIVRÉ à la Station ! (La connexion fonctionne)");
  } else {
    Serial.println("ÉCHEC - Aucune réponse de la station (Canal ou MAC incorrect)");
  }
}

void callbackReceptionRadio(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(donneesStation)) {
    // Copie la structure reçue
    memcpy(&donneesStation, incomingData, sizeof(donneesStation));
    
    // MISE À JOUR FORCÉE
    // On ne fait pas de condition, on met à jour la globale systématiquement
    rssiStation = donneesStation.rssi; 

    Serial.print("RSSI Reçu : ");
    Serial.println(rssiStation);
    
    // ... reste du code ...
  }
}

void initGestionSansFil() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ESP-NOW] Erreur d'initialisation sur le Robot");
    return;
  }

  // Enregistrement des fonctions d'interruption
  esp_now_register_send_cb(callbackEnvoiRadio);
  esp_now_register_recv_cb(callbackReceptionRadio);

  // Liaison radio avec l'ESP32-S3 de la Station
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  
  memcpy(peerInfo.peer_addr, macStation, 6);
  peerInfo.channel = 1;         // Forcé sur le canal 1 pour s'aligner avec le Wi-Fi
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA; // Spécifique ESP32-S3 pour l'antenne cliente
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[ESP-NOW] Impossible d'ajouter la Station comme peer");
    return;
  }

  // Statuts initiaux
  donneesStation.contactsAlimentes = false;
  donneesStation.correctionTrajectoire = 0;
  donneesStation.rssi = -100;
  strcpy(donneesStation.message, "RECHERCHE BASE");
}

void updateGestionSansFil() {
  // Cadencement de l'envoi toutes les secondes
  if (millis() - dernierEnvoiRadioMs >= 1000) {
    dernierEnvoiRadioMs = millis();

    // Lecture de la batterie
    donneesRobot.tensionBatterie = lireTensionBatterie();
    donneesRobot.pourcentage = lirePourcentageBatterie(donneesRobot.tensionBatterie);

    // Traduction textuelle du mode pour la station
    if (modeRobot == MODE_AUTO) {
      strcpy(donneesRobot.etat, "AUTO");
    } else if (modeRobot == MODE_RETOUR_BASE) {
      strcpy(donneesRobot.etat, "RETOUR_BASE");
    } else {
      strcpy(donneesRobot.etat, "MANUEL");
    }

    // Envoi du paquet binaire compressé
    esp_now_send(macStation, (uint8_t *) &donneesRobot, sizeof(donneesRobot));
  }
}