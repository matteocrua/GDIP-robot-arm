#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "IK_solver.h"

extern const ArmGeometry ARM;
extern IKSolver solver;
extern Adafruit_PWMServoDriver pwm;

extern const uint8_t CH_BASE;
extern const uint8_t CH_J1;
extern const uint8_t CH_J2;
extern const uint8_t CH_J3;

extern const int SERVO_FREQ;
extern const int PULSE_MIN_US;
extern const int PULSE_MAX_US;

extern const float JOINT_REF[3];
extern const float BASE_OFFSET;
extern const float OFFSET[3];
extern const float DIR[3];
extern const float BASE_DIR;

#endif
