#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h> // Indispensable pour la fonction esp_wifi_set_channel
#include <cstring>
#include "Gestion_Wireless.h"

// Adresse MAC du robot
uint8_t macRobot[6] = {0x90, 0xE5, 0xB1, 0xD5, 0xF4, 0xFC};

StructRobot donneesRobot;
StructStation donneesStation;

int calculerAlignement() {
  return 0; 
}

// Fonction de réception (Callback)
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(donneesRobot)) {
    memcpy(&donneesRobot, incomingData, sizeof(donneesRobot));
    
    Serial.print("\n[Station] Mode Robot : "); Serial.println(donneesRobot.modeActuel);
    Serial.printf("[Station] Batterie : %.2f V (%d%%)\n", donneesRobot.tensionBatterie, donneesRobot.pourcentageBatterie);
    
    // Logique de décision
    if (strcmp(donneesRobot.modeActuel, "RETOUR_BASE") == 0) {
      int alignement = calculerAlignement();
      donneesStation.correctionTrajectoire = alignement;
      donneesStation.contactsAlimentes = false;

      if (alignement == 1)      strcpy(donneesStation.message, "PIVOTE A GAUCHE");
      else if (alignement == 2) strcpy(donneesStation.message, "PIVOTE A DROITE");
      else                      strcpy(donneesStation.message, "DROIT DEVANT");

    } else if (strcmp(donneesRobot.modeActuel, "EN_CHARGE") == 0) {
      donneesStation.correctionTrajectoire = 0;
      donneesStation.contactsAlimentes = true; 
      strcpy(donneesStation.message, "ROBOT EN CHARGE");

    } else {
      donneesStation.correctionTrajectoire = 0;
      donneesStation.contactsAlimentes = false;
      strcpy(donneesStation.message, "STATION EN VEILLE");
    }

    // Réponse au robot
    esp_now_send(mac, (uint8_t *)&donneesStation, sizeof(donneesStation));
    Serial.print("[Station] Reponse envoyee : ");
    Serial.println(donneesStation.message);
  }
}

// Initialisation sans fil avec forçage du Canal 1
void initSansFilStation() {
  // 1. Démarrage du mode Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Déconnexion des réseaux environnants pour libérer le canal
  delay(100);
  
  // 2. FORCE L'ANTENNE SUR LE CANAL 1 (Ligne critique pour l'ESP32-S3)
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); 
  
  // 3. Affichage des infos de la station dans le moniteur série
  Serial.println("-------------------------------------------");
  Serial.print("[Station] Wi-Fi configure sur le CANAL : ");
  Serial.println(WiFi.channel()); // Doit afficher 1
  Serial.print("[Station] Adresse MAC de ma Station   : ");
  Serial.println(WiFi.macAddress());
  Serial.println("-------------------------------------------");

  // 4. Initialisation d'ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ESP-NOW] Echec initialisation Station");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);

  // 5. Configuration de l'appairage avec le Robot sur le Canal 1
  esp_now_peer_info_t infoRobot;
  memset(&infoRobot, 0, sizeof(infoRobot));
  memcpy(infoRobot.peer_addr, macRobot, 6);
  infoRobot.channel = 1;         // Force l'écoute/envoi du peer sur le canal 1
  infoRobot.encrypt = false;
  infoRobot.ifidx = WIFI_IF_STA; // Spécifique et obligatoire pour l'ESP32-S3

  if (esp_now_add_peer(&infoRobot) != ESP_OK) {
    Serial.println("[ESP-NOW] Impossible d'associer le Robot");
    return;
  }
  
  Serial.println("=== CONFIGURATION SANS FIL COUPLÉE AU CANAL 1 ===");
}