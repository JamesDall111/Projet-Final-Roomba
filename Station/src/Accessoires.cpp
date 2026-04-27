#include "Accessoires.h"

void initAccessoires() {
  // ===== BUZZER PWM =====
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  // ===== LED =====
  pinMode(PIN_LED_DATA, OUTPUT);
  digitalWrite(PIN_LED_DATA, LOW);
}

// ===== BUZZER =====
void beepCourt() {
  ledcWrite(BUZZER_CHANNEL, 128); // son
  delay(100);
  ledcWrite(BUZZER_CHANNEL, 0);   // stop
}

// ===== LED (temporaire) =====
void ledOff() {
  digitalWrite(PIN_LED_DATA, LOW);
}

void ledOrange() {
  // ⚠️ pour l’instant juste ON
  digitalWrite(PIN_LED_DATA, HIGH);
}

void ledVert() {
  // ⚠️ idem (ta LED ne permet pas de changer couleur pour l’instant)
  digitalWrite(PIN_LED_DATA, HIGH);
}