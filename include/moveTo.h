#ifndef MOVE_TO_H
#define MOVE_TO_H

#include <Arduino.h>

void setServoAngle(uint8_t channel, float degrees);
bool moveTo(float reachMm, float heightMm, float baseAngleDeg);

#endif
