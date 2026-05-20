#include "Accessoires.h"

void initAccessoires() {
  // ===== BUZZER =====
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  // ===== LED =====
  pinMode(PIN_LED, OUTPUT);
  ledOff();

  // ===== IR LED =====
  pinMode(IR_LED_1, OUTPUT);
  pinMode(IR_LED_2, OUTPUT);
  pinMode(IR_LED_3, OUTPUT);
  pinMode(IR_LED_4, OUTPUT);
  pinMode(IR_LED_5, OUTPUT);

  IR_Off();
}

// ===== BUZZER =====
void beepCourt() {
  ledcWrite(BUZZER_CHANNEL, 128);
  delay(100);
  ledcWrite(BUZZER_CHANNEL, 0);
}

// ===== LED =====
void ledOff() {
  digitalWrite(PIN_LED, LOW);
}

void ledBleu() {
  digitalWrite(PIN_LED, HIGH);
}

void ledVert() {
  digitalWrite(PIN_LED, LOW);
}

// ===== IR LED =====
void IR_On() {
  digitalWrite(IR_LED_1, HIGH);
  digitalWrite(IR_LED_2, HIGH);
  digitalWrite(IR_LED_3, HIGH);
  digitalWrite(IR_LED_4, HIGH);
  digitalWrite(IR_LED_5, HIGH);
}

void IR_Off() {
  digitalWrite(IR_LED_1, LOW);
  digitalWrite(IR_LED_2, LOW);
  digitalWrite(IR_LED_3, LOW);
  digitalWrite(IR_LED_4, LOW);
  digitalWrite(IR_LED_5, LOW);
}