#ifndef ACCESSOIRES_H
#define ACCESSOIRES_H

#include <Arduino.h>

// ===== PINS =====
#define PIN_BUZZER 14
#define PIN_LED_DATA 26   // ton fil LED actuel

// ===== BUZZER (ESP32 PWM) =====
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8

// ===== INIT =====
void initAccessoires();

// ===== BUZZER =====
void beepCourt();

// ===== LED =====
void ledOff();
void ledOrange();
void ledVert();

#endif