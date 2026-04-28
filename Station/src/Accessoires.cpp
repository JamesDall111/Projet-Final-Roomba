#include "Accessoires.h"

void initAccessoires() {
  // ===== BUZZER PWM =====
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  // ===== LED =====
  pinMode(PIN_LED, OUTPUT);

  // état de départ
  ledOff();
}

// ===== BUZZER =====
void beepCourt() {
  ledcWrite(BUZZER_CHANNEL, 128);
  delay(100);
  ledcWrite(BUZZER_CHANNEL, 0);
}

// ===== LED 1 PIN =====
// Selon ton branchement :
// Blanc → 3.3V
// Noir  → GND
// Rouge → PIN_LED avec résistance

void ledOff() {
  digitalWrite(PIN_LED, LOW);
}

void ledBleu() {
  digitalWrite(PIN_LED, HIGH);
}

void ledVert() {
  digitalWrite(PIN_LED, LOW);
}
// ===== TEST =====
void ledTestSequence() {
  Serial.println("LED BLEU");
  ledBleu();
  delay(2000);

  Serial.println("LED VERT + BEEP");
  ledVert();
  beepCourt();
  delay(2000);

  Serial.println("LED OFF");
  ledOff();
  delay(2000);
}