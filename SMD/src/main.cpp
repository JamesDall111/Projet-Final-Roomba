#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 15

// ===== MOTEUR =====
const int PIN_ENA = 14;
const int PIN_IN1 = 32;
const int PIN_IN2 = 33;

const int PWM_FREQ = 1000;
const int PWM_RESOLUTION = 8;
const int PWM_CHANNEL = 0;
const int VITESSE_MOTEUR = 200;

// ===== TELECOMMANDE =====
const uint16_t REMOTE_ADDRESS = 0x4587;

const uint8_t CMD_AVANCE = 0x28;
const uint8_t CMD_DROITE = 0x2A;
const uint8_t CMD_GAUCHE = 0x2E;
const uint8_t CMD_RECULE = 0x2C;

const unsigned long STOP_AUTOMATIQUE_MS = 1200;
unsigned long dernierOrdreValide = 0;

// ===== MOTEUR =====
void vitesse(int v) {
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  ledcWrite(PWM_CHANNEL, v);
}

void stopMoteur() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  vitesse(0);
  Serial.println("STOP");
}

void avancer() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  vitesse(VITESSE_MOTEUR);
  Serial.println("AVANCE");
}

void reculer() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  vitesse(VITESSE_MOTEUR);
  Serial.println("RECULE");
}

void droite() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  vitesse(VITESSE_MOTEUR);
  Serial.println("DROITE");
}

void gauche() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  vitesse(VITESSE_MOTEUR);
  Serial.println("GAUCHE");
}

void commande(uint8_t cmd) {
  if (cmd == CMD_AVANCE) avancer();
  else if (cmd == CMD_DROITE) droite();
  else if (cmd == CMD_GAUCHE) gauche();
  else if (cmd == CMD_RECULE) reculer();
  else {
    Serial.print("Commande inconnue: 0x");
    Serial.println(cmd, HEX);
    return;
  }

  dernierOrdreValide = millis();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_ENA, PWM_CHANNEL);

  stopMoteur();
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Pret");
}

void loop() {
  if (dernierOrdreValide && millis() - dernierOrdreValide > STOP_AUTOMATIQUE_MS) {
    stopMoteur();
    dernierOrdreValide = 0;
  }

  if (!IrReceiver.decode()) return;

  Serial.print("Protocol=");
  Serial.print(IrReceiver.decodedIRData.protocol);
  Serial.print(" Addr=0x");
  Serial.print(IrReceiver.decodedIRData.address, HEX);
  Serial.print(" Cmd=0x");
  Serial.println(IrReceiver.decodedIRData.command, HEX);

  if (IrReceiver.decodedIRData.protocol == NEC &&
      IrReceiver.decodedIRData.address == REMOTE_ADDRESS) {
    commande(IrReceiver.decodedIRData.command);
  } else {
    Serial.println("Bruit / autre signal");
  }

  IrReceiver.resume();
}