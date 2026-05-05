#include "Lcd.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Adafruit_SSD1306 display(128, 64, &Wire, -1);

void initDisplay() {
    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 fail");
        while (true);
    }

    display.clearDisplay();
}

void updateDisplay(int percent) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Station");

    display.setCursor(10, 22);
    display.print(percent);
    display.print("%");

    display.drawRect(10, 40, 80, 20, WHITE);
    display.fillRect(90, 46, 5, 8, WHITE);

    int levelWidth = map(percent, 0, 100, 0, 76);
    display.fillRect(12, 42, levelWidth, 16, WHITE);

    display.display();
}
    
