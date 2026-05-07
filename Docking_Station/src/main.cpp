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

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (true) {}
  }

  initAccessoires();
    IR_On();

  display.clearDisplay();
  display.display();

  beepCourt();
  ledBleu();   // bleu au départ

}

void loop() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Station de charge");

  display.setCursor(0, 12);

  if (percent < 60) {
    display.println("Etat: EN CHARGE");
    ledBleu();     // toujours bleu sous 60%
  } else {
    display.println("Etat: CHARGE OK");
    ledVert();     // vert à partir de 60%
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

  // petit bip seulement au passage à 60%
  if (percent == 60) {
    beepCourt();
  }

  percent++;

  if (percent > 100) {
    percent = 0;
    ledBleu();   // retour bleu quand ça recommence à 0/1%
  }

  delay(150);
}