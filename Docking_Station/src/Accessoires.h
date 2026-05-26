#ifndef ACCESSOIRES_H
#define ACCESSOIRES_H

#include <Arduino.h>

// ===== PINS =====
#define PIN_BUZZER 2
#define PIN_LED 7

// ===== IR =====
#define IR_M3 4   // 2 LED extremites (NEC)
#define IR_M2 1   // LED centre (38 kHz burst)

// ===== BUZZER PWM =====
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8

// ===== IR PWM =====
#define IR_CHANNEL_M2 1
#define IR_FREQ 38000
#define IR_RESOLUTION 8
#define IR_DUTY 128

// ===== NEC =====
#define IR_ADDRESS 0x4587
#define CMD_CENTRE 0x28
#define CMD_LEFT   0x2E
#define CMD_RIGHT  0x2A

void initAccessoires();

// ===== BUZZER =====
void beepCourt();

// ===== LED =====
void ledOff();
void ledBleu();
void ledVert();

// ===== IR M2 =====
void IR_M2_Burst(unsigned long temps_us);

// ===== IR NEC M3 =====
void IR_SendCentre();
void IR_SendLeft();
void IR_SendRight();

#endif