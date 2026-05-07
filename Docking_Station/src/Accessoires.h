#ifndef ACCESSOIRES_H
#define ACCESSOIRES_H

#include <Arduino.h>

// ===== PINS =====
#define PIN_BUZZER 2
#define PIN_LED 7

// IR LEDs / transistors
#define IR_M3 4
#define IR_M2 1

/*#define IR_LED_2 26
#define IR_LED_3 25
#define IR_LED_4 27
#define IR_LED_5 13*/

// ===== BUZZER PWM =====
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8

// ===== IR PWM 38 kHz =====
#define IR_CHANNEL_M3 1
#define IR_CHANNEL_M2 2
#define IR_FREQ 38000
#define IR_RESOLUTION 8
#define IR_DUTY 128

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