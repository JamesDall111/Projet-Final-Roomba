#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Accessoires.h"
#include "Lcd.h"

//#include "CommWifi.h"

int percent = 0;

void setup() {

  Serial.begin(115200);
  delay(1000);

  // ===== I2C =====
  Wire.begin();

  // ===== OLED =====
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("SSD1306 failed");

    while (true) {
    }
  }

  // ===== ACCESSOIRES =====
  initAccessoires();

  // ===== OLED =====
  display.clearDisplay();
  display.display();

  // ===== START =====
  beepCourt();
  ledBleu();

  Serial.println("=== STATION DEMARREE ===");
}

void loop() {

  // =========================================
  // IR
  // =========================================

  // burst 38 kHz centre
  IR_M2_Burst(600);

  // NEC extremites
  IR_SendCentre();

  // =========================================
  // OLED
  // =========================================

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Station de charge");

  display.setCursor(0, 12);

  // ===== ETAT =====
  if (percent < 60) {

    display.println("Etat: EN CHARGE");
    ledBleu();

  } else {

    display.println("Etat: CHARGE OK");
    ledVert();
  }

  // ===== POURCENTAGE =====
  display.setTextSize(2);

  display.setCursor(10, 25);
  display.print(percent);
  display.print("%");

  // ===== BATTERIE =====
  display.drawRect(10, 50, 80, 12, SSD1306_WHITE);

  display.fillRect(90, 53, 5, 6, SSD1306_WHITE);

  int levelWidth = map(percent, 0, 100, 0, 76);

  display.fillRect(12, 52, levelWidth, 8, SSD1306_WHITE);

  display.display();

  // ===== BEEP =====
  if (percent == 60) {
    beepCourt();
  }

  // ===== INCREMENT =====
  percent++;

  if (percent > 100) {

    percent = 0;

    ledBleu();
  }

  delay(100);
}