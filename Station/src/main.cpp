#include <Arduino.h>
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

// ====== CONFIG SIMPLE ======
#define SDA_PIN 21
#define SCL_PIN 22

#define MODE_TEST 0
#define MODE_REMOTE 1

int mode = MODE_TEST;   // change à MODE_REMOTE plus tard

int percent = 0;
int percentRobot = 75;  // valeur fake reçue du robot plus tard

Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define DIS_WIDTH 128
#define DIS_HEIGHT 64

void scanI2C() {
  Serial.println("Scan I2C...");

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);

    if (Wire.endTransmission() == 0) {
      Serial.print("Trouve: 0x");
      Serial.println(address, HEX);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);  // SDA/SCL pour ESP32

  scanI2C();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  delay(2000);
  display.clearDisplay();
}

void loop() {

  // ====== SOURCE DU POURCENTAGE ======
  if (mode == MODE_TEST) {
    percent++;

    if (percent > 100) {
      percent = 0;
    }
  }

  if (mode == MODE_REMOTE) {
    percent = percentRobot; // plus tard: valeur reçue du robot
  }

  // ====== AFFICHAGE OLED ======
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  if (mode == MODE_TEST) {
    display.println("Test batterie");
  } else {
    display.println("Batterie robot");
  }

  display.setCursor(10, 22);
  display.print(percent);
  display.print("%");

  display.drawRect(10, 40, 80, 20, WHITE);
  display.fillRect(90, 46, 5, 8, WHITE);

  int levelWidth = map(percent, 0, 100, 0, 76);
  display.fillRect(12, 42, levelWidth, 16, WHITE);

  display.display();

  delay(80);
}