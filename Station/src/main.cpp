#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Accessoires.h"
#include "Lcd.h"

#define MODE_TEST 1

int percent = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (true) {
    }
  }

  initAccessoires();

  display.clearDisplay();
  display.display();

  beepCourt();
  ledOrange();
}

void loop() {

#if MODE_TEST

  // ===== TEST ORANGE =====
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("TEST ACCESSOIRES");

  display.setCursor(0, 20);
  display.println("LED: ORANGE");

  display.setCursor(0, 35);
  display.println("BUZZER: BEEP");

  display.display();

  ledOrange();
  beepCourt();
  delay(1000);

  // ===== TEST VERT =====
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("TEST ACCESSOIRES");

  display.setCursor(0, 20);
  display.println("LED: VERT");

  display.setCursor(0, 35);
  display.println("BUZZER: BEEP");

  display.display();

  ledVert();
  beepCourt();
  delay(1000);

  // ===== TEST OFF =====
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("TEST ACCESSOIRES");

  display.setCursor(0, 20);
  display.println("LED: OFF");

  display.setCursor(0, 35);
  display.println("BUZZER: OFF");

  display.display();

  ledOff();
  delay(1000);

#else

  // ===== MODE NORMAL =====

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("En chargement !");

  display.setCursor(10, 22);
  display.print(percent);
  display.print("%");

  display.drawRect(10, 40, 80, 20, SSD1306_WHITE);
  display.fillRect(90, 46, 5, 8, SSD1306_WHITE);

  int levelWidth = map(percent, 0, 100, 0, 76);
  display.fillRect(12, 42, levelWidth, 16, SSD1306_WHITE);

  display.display();

  if (percent < 50) {
    ledOrange();
  } else {
    ledVert();
  }

  percent++;
  if (percent > 100) percent = 0;

  delay(80);

#endif
}