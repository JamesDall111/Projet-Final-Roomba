#include "CommWifi.h"
#include <WiFi.h>
#include <esp_now.h>

typedef struct MessageBatterie {
  int batterie;
} MessageBatterie;

MessageBatterie message;

static int batterie = 0;
static bool dataRecu = false;

// ===== CALLBACK =====
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&message, data, sizeof(message));

  batterie = message.batterie;

  if (batterie < 0) batterie = 0;
  if (batterie > 100) batterie = 100;

  dataRecu = true;

  Serial.print("Recu: ");
  Serial.print(batterie);
  Serial.println("%");
}

// ===== INIT =====
void initCommWifi() {
  WiFi.mode(WIFI_STA);

  Serial.print("MAC STATION: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erreur ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

// ===== GETTERS =====
int getBatterie() {
  return batterie;
}

bool isDataReceived() {
  return dataRecu;
}