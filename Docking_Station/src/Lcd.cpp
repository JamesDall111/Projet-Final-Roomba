#include "Lcd.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void initDisplay() {
  Wire.begin(5, 6); // SDA = 5, SCL = 6

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 fail");
    while (true) {}
  }

  display.clearDisplay();
  display.display();
}

void updateDisplay(int percent, const char* mode) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Station de charge124"); // Ton texte d'origine

  display.setCursor(0, 12);

  // ===== ÉTAT DYNAMIQUE REPRIS DE TON LOGICIEL =====
  if (strcmp(mode, "EN_CHARGE") == 0) {
    display.println("Etat: EN CHARGE");
  } else if (strcmp(mode, "RETOUR_BASE") == 0) {
    display.println("Etat: RECHERCHE...");
  } else if (percent < 20 && percent > 0) {
    display.println("Etat: BATT. FAIBLE");
  } else {
    display.println("Etat: CHARGE OK");
  }

  // ===== AFFICHAGE DE L'ÉTAT DES INFRAROUGES =====
  display.setCursor(0, 22);
  if (strcmp(mode, "RETOUR_BASE") == 0 || (percent < 20 && percent > 0)) {
    display.println("IR: ON");
  } else {
    display.println("IR: OFF");
  }

  // ===== POURCENTAGE (TAILLE 2) =====
  display.setTextSize(2);
  display.setCursor(10, 35);
  display.print(percent);
  display.print("%");

  // ===== TON DESSIN DE BATTERIE D'ORIGINE =====
  display.drawRect(10, 55, 80, 8, SSD1306_WHITE);
  display.fillRect(90, 57, 5, 4, SSD1306_WHITE);

  int levelWidth = map(percent, 0, 100, 0, 76);
  display.fillRect(12, 57, levelWidth, 4, SSD1306_WHITE);

  display.display();
}