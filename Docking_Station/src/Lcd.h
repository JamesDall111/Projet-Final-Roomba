#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

void initDisplay();
void updateDisplay(int percent, const char* mode); // Changé ici

#endif