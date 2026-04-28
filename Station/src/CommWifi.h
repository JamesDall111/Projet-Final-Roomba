#ifndef COMMWIFI_H
#define COMMWIFI_H

#include <Arduino.h>

void initCommWifi();
int getBatterie();
bool isDataReceived();

#endif


/*main modif 
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Accessoires.h"
#include "Lcd.h"
#include "CommWifi.h"

int percent = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (true) {}
  }

  initAccessoires();
  initCommWifi();

  display.clearDisplay();
  display.display();

  ledBleu();
}

void loop() {

  if (isDataReceived()) {
    percent = getBatterie();
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Station");

  display.setCursor(0, 12);

  if (percent < 60) {
    display.println("EN CHARGE");
    ledBleu();
  } else {
    display.println("CHARGE OK");
    ledVert();
  }

  display.setTextSize(2);
  display.setCursor(10, 25);
  display.print(percent);
  display.print("%");

  display.drawRect(10, 50, 80, 12, SSD1306_WHITE);
  display.fillRect(90, 53, 5, 6, SSD1306_WHITE);

  int levelWidth = map(percent, 0, 100, 0, 76);
  display.fillRect(12, 52, levelWidth, 8, SSD1306_WHITE);

  display.display();

  delay(200);
}
*/