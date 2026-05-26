#include <Arduino.h>
#include <Wire.h>

#include "Gestion_Wireless.h"
#include "Accessoires.h"
#include "Lcd.h"

int dernierPourcentage = -1;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }

  Serial.println("\n=== STATION DEMARRAGE ===");

  initDisplay();          // Écran OLED
  initAccessoires();      // Buzzer, LED RGB, IR
  initSansFilStation();   // ESP-NOW

  beepCourt();
  ledBleu();

  Serial.println("=== STATION PRETE ===");
}

void loop() {

  int pourcentReel = donneesRobot.pourcentageBatterie;
  const char* modeActuel = donneesRobot.modeActuel;

  // ===== IR STATION =====
  // Envoie seulement le même type de signal qu'une télécommande NEC
  if (strcmp(modeActuel, "RETOUR_BASE") == 0 || (pourcentReel < 20 && pourcentReel > 0)) {

    IR_SendCentre();

  }

  // ===== LED RGB =====
  if (strcmp(modeActuel, "RETOUR_BASE") == 0) {

    ledBleu();

  } else {

    if (pourcentReel < 60) {
      ledBleu();
    } else {
      ledVert();
    }
  }

  // ===== OLED =====
  updateDisplay(pourcentReel, modeActuel);

  // ===== BUZZER =====
  if (pourcentReel == 60 && dernierPourcentage != 60) {
    beepCourt();
  }

  dernierPourcentage = pourcentReel;

  delay(150);
}