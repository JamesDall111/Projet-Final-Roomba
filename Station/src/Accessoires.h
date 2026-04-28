#ifndef ACCESSOIRES_H
#define ACCESSOIRES_H

#include <Arduino.h>

// ===== PINS =====
#define PIN_BUZZER 14
#define PIN_LED 32

// ===== BUZZER PWM =====
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8

void initAccessoires();

void beepCourt();

void ledOff();
void ledBleu();
void ledVert();

void ledTestSequence();

#endif