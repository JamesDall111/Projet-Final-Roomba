#include <Arduino.h> 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Accessoires.h"
#include "Lcd.h"

int percent = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();

  // 🔥 IMPORTANT → initialisation écran
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (true) {}
  }

  initAccessoires();

  display.clearDisplay();
  display.display();

  beepCourt();
  ledBleu();

  // IR_On(); // si tu veux
}

void loop() {

  // ===== LOGIQUE LED =====
  if (percent < 60) {
    ledBleu();
  } else {
    ledVert();
  }

  // ===== AFFICHAGE =====
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Station de charge");

  display.setCursor(0, 12);

  if (percent < 60) {
    display.println("Etat: EN CHARGE");
  } else {
    display.println("Etat: CHARGE OK");
  }

  display.setCursor(0, 22);
  display.println("IR: ON");

  display.setTextSize(2);
  display.setCursor(10, 35);
  display.print(percent);
  display.print("%");

  display.drawRect(10, 55, 80, 8, SSD1306_WHITE);
  display.fillRect(90, 57, 5, 4, SSD1306_WHITE);

  int levelWidth = map(percent, 0, 100, 0, 76);
  display.fillRect(12, 57, levelWidth, 4, SSD1306_WHITE);

  display.display();

  // ===== BUZZER =====
  if (percent == 60) {
    beepCourt();
  }

  // ===== INCRÉMENT =====
  percent++;

  if (percent > 100) {
    percent = 0;
  }

  delay(150);
}