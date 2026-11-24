#include "global.h"

const ArmGeometry ARM = {
  90.0f,
  147.0f,
  182.5f,
  50.0f
};

IKSolver solver(ARM);
Adafruit_PWMServoDriver pwm(0x40);

const uint8_t CH_BASE = 0;
const uint8_t CH_J1 = 1;
const uint8_t CH_J2 = 2;
const uint8_t CH_J3 = 3;

const int SERVO_FREQ = 50;
const int PULSE_MIN_US = 500;
const int PULSE_MAX_US = 2500;

const float JOINT_REF[3] = {90.0f, -90.0f, 0.0f};
const float BASE_OFFSET = 80.0f;
const float OFFSET[3] = {81.0f, 81.0f, 78.0f};
const float DIR[3] = {1.0f, -1.0f, 1.0f};
const float BASE_DIR = 1.0f;
