#ifndef GLOBAL_H
#define GLOBAL_H

// Global header files
#include "IK_solver.h"

// Global Libraries
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Global initialization of variables and constants for the robot arm

// Arm geometry for the Lynx Motion AL5D Robot Arm
extern const ArmGeometry ARM;
extern IKSolver solver;
extern Adafruit_PWMServoDriver pwm;

// Servo channel assignments
extern const uint8_t CH_BASE;
extern const uint8_t CH_J1;
extern const uint8_t CH_J2;
extern const uint8_t CH_J3;

// Servo control parameters
extern const int SERVO_FREQ;
extern const int PULSE_MIN_US;
extern const int PULSE_MAX_US;

// Joint reference angles for the robot arm
extern const float JOINT_REF[3];
extern const float BASE_OFFSET;
extern const float OFFSET[3];
extern const float DIR[3];
extern const float BASE_DIR;

#endif
