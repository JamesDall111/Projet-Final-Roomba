#ifndef ACCESSOIRES_H
#define ACCESSOIRES_H

#include <Arduino.h>

// ===== PINS =====
#define PIN_BUZZER 14
#define PIN_LED 26

// IR LEDs (choisis tes GPIO libres)
#define IR_LED_1 32
#define IR_LED_2 33
#define IR_LED_3 25
#define IR_LED_4 27
#define IR_LED_5 13

// ===== BUZZER PWM =====
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8

void initAccessoires();

// ===== BUZZER =====
void beepCourt();

// ===== LED =====
void ledOff();
void ledBleu();
void ledVert();

// ===== IR =====
void IR_On();
void IR_Off();

#endif