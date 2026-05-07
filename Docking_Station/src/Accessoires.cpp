#include "Accessoires.h"

void initAccessoires() {
  // ===== BUZZER =====
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  // ===== LED =====
  pinMode(PIN_LED, OUTPUT);
  ledOff();

  // ===== IR LED 38 kHz - M3 =====
  ledcSetup(IR_CHANNEL_M3, IR_FREQ, IR_RESOLUTION);
  ledcAttachPin(IR_M3, IR_CHANNEL_M3);
  ledcWrite(IR_CHANNEL_M3, 0);

  // ===== IR LED 38 kHz - M2 =====
  ledcSetup(IR_CHANNEL_M2, IR_FREQ, IR_RESOLUTION);
  ledcAttachPin(IR_M2, IR_CHANNEL_M2);
  ledcWrite(IR_CHANNEL_M2, 0);

/*  pinMode(IR_LED_2, OUTPUT);
  pinMode(IR_LED_3, OUTPUT);
  pinMode(IR_LED_4, OUTPUT);
  pinMode(IR_LED_5, OUTPUT);

  IR_Off();*/
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
  ledcWrite(IR_CHANNEL_M3, IR_DUTY);
  ledcWrite(IR_CHANNEL_M2, IR_DUTY);

 /* digitalWrite(IR_LED_2, HIGH);
  digitalWrite(IR_LED_3, HIGH);
  digitalWrite(IR_LED_4, HIGH);
  digitalWrite(IR_LED_5, HIGH);*/
}

void IR_Off() {
  ledcWrite(IR_CHANNEL_M3, 0);
  ledcWrite(IR_CHANNEL_M2, 0);

  /*digitalWrite(IR_LED_2, LOW);
  digitalWrite(IR_LED_3, LOW);
  digitalWrite(IR_LED_4, LOW);
  digitalWrite(IR_LED_5, LOW);*/
}