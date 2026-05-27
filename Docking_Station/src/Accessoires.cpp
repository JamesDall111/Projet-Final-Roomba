#include "Accessoires.h"
#include <IRremote.hpp>


void initAccessoires() {

  // ===== BUZZER =====
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  // ===== LED =====
  pinMode(PIN_LED, OUTPUT);
  ledOff();

  // ===== IR M2 PWM =====
  ledcSetup(IR_CHANNEL_M2, IR_FREQ, IR_RESOLUTION);
  ledcAttachPin(IR_M2, IR_CHANNEL_M2);
  ledcWrite(IR_CHANNEL_M2, 0);

  // ===== IRremote =====
  IrSender.begin(IR_M3);

  Serial.println("Accessoires OK");
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

// ===== IR BURST M2 =====
void IR_M2_Burst(unsigned long temps_us) {

  ledcWrite(IR_CHANNEL_M2, IR_DUTY);

  delayMicroseconds(temps_us);

  ledcWrite(IR_CHANNEL_M2, 0);
}

// ===== IR NEC =====
void IR_SendCentre() {
  IrSender.sendNEC(IR_ADDRESS, CMD_CENTRE, 0);
}

void IR_SendLeft() {
  IrSender.sendNEC(IR_ADDRESS, CMD_LEFT, 0);
}

void IR_SendRight() {
  IrSender.sendNEC(IR_ADDRESS, CMD_RIGHT, 0);
}