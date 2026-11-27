#ifndef MOVE_TO_H
#define MOVE_TO_H

// Header files needed for moveTo.cpp
#include "global.h"

// Libraries needed for moveTo.cpp
#include <Arduino.h>
#include <math.h>

// Function declarations
void setServoAngle(uint8_t channel, float degrees);
bool moveTo(float reachMm, float heightMm, float baseAngleDeg);



#endif
