#ifndef WEBCONTROL_H
#define WEBCONTROL_H

enum ModeRobot {
  MODE_MANUEL,
  MODE_AUTO,
  MODE_RETOUR_BASE
};

extern ModeRobot modeRobot;

void initWebControl();
void updateWebControl();

#endif