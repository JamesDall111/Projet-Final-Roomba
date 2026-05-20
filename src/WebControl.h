#ifndef WEBCONTROL_H
#define WEBCONTROL_H

enum ModeRobot {
  MODE_MANUEL,
  MODE_AUTO
};

extern ModeRobot modeRobot;

void initWebControl();
void updateWebControl();

#endif