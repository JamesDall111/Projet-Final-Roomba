#include <Arduino.h>

#include "Telecommande_IR.h"
#include "Moteur.h"
#include "Accessoires.h"
#include "WebControl.h"
#include "Capteur_IR.h"

bool commandeValide(uint8_t cmd) {
  return cmd == IR_CMD_MODE;  // Seulement le bouton MODE
}

void gererCommandeTelecommande(uint8_t cmd, bool repetition) {
  if (repetition) {
    return;
  }

  if (modeRobot == MODE_MANUEL) {
    modeRobot = MODE_AUTO;
    accessoiresOn();
    Serial.println("MODE AUTO");
  }
  else if (modeRobot == MODE_AUTO) {
    modeRobot = MODE_RETOUR_BASE;
    accessoiresOff();
    moveCmd = CMD_STOP;
    stopRoues();
    Serial.println("MODE RETOUR BASE");
  }
  else {
    modeRobot = MODE_MANUEL;
    accessoiresOff();
    moveCmd = CMD_STOP;
    stopRoues();
    Serial.println("MODE MANUEL");
  }
}