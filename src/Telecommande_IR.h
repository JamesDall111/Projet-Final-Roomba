#ifndef TELECOMMANDE_IR_H
#define TELECOMMANDE_IR_H

#include <Arduino.h>

#define REMOTE_ADDRESS 0x4587

#define IR_CMD_AVANCE 0x28
#define IR_CMD_DROITE 0x2A
#define IR_CMD_GAUCHE 0x2E
#define IR_CMD_RECULE 0x2C
#define IR_CMD_MODE   0x17

bool commandeValide(uint8_t cmd);
void gererCommandeTelecommande(uint8_t cmd, bool repetition);

#endif