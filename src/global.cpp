// Header files
#include "global.h"

// Global definitions for variables and constants for the robot arm

// Arm geometry for the Lynx Motion AL5D Robot Arm
const ArmGeometry ARM = {
  90.0f,
  147.0f,
  293.0f,// 182.5f,
  68.0f
};

// Initialize the inverse kinematics solver and PWM servo driver
IKSolver solver(ARM);
Adafruit_PWMServoDriver pwm(0x40);

// Servo channel assignments
const uint8_t CH_BASE = 12;
const uint8_t CH_J1 = 13;
const uint8_t CH_J2 = 14;
const uint8_t CH_J3 = 15;

// Servo control parameters
const int SERVO_FREQ = 50;
const int PULSE_MIN_US = 500;
const int PULSE_MAX_US = 2500;

// Joint reference angles for the robot arm
const float JOINT_REF[3] = {90.0f, -90.0f, 0.0f};
const float BASE_OFFSET = 80.0f;
const float OFFSET[3] = {81.0f, 81.0f, 78.0f};
const float DIR[3] = {1.0f, -1.0f, 1.0f};
const float BASE_DIR = 1.0f;
