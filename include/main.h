#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "ik_solver.h"

// ---------- Arm geometry (mm) ----------
const ArmGeometry ARM = {
  70.0f,    // base height
  146.1f,   // l1 (J1 - J2)
  187.3f,   // l2 (J2 - J3)
  50.0f     // l3 (J3 - end effector)
};

IKSolver solver(ARM);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// ---------- Servo setup ----------
const uint8_t CH_BASE = 0, CH_J1 = 1, CH_J2 = 2, CH_J3 = 3;   // match your wiring
const int SERVO_FREQ   = 50;     // Hz
const int PULSE_MIN_US = 500;    // pulse at 0 deg   (tune for your servos)
const int PULSE_MAX_US = 2500;   // pulse at 180 deg

// Servo angle = OFFSET + DIR * jointAngle. Calibrate these for your build.
const float JOINT_REF[3] = {90.0f, -90.0f, 0.0f};

const float BASE_OFFSET = 80.0f;                  // base facing forward
const float OFFSET[3] = {81.0f, 81.0f, 83.0f};    // shoulder, elbow, wrist (wrist is a guess)
const float DIR[3]    = { 1.0f,  -1.0f,  1.0f};
const float BASE_DIR = 1.0f;   // flip to -1 if the base turns the wrong way

static float currentServoAngles[4];










/* // includes
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "IK_solver.h"
#include "math.h"

// definitions
#define SERVO_ADDR 0x40 // I2C address of the PCA9685
#define SERVO_FREQ 50   // Standard hobby-servo frequency
#define SERVOMIN 600    // Minimum pulse length
#define SERVOMAX 2400   // Maximum pulse length
#define SERVOCENTRE 1500 // Centre pulse length 

// create a PCA9685 object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(SERVO_ADDR);

// constants
const int servo_count = 4; // Number of servos (joints) in the robot arm
const int servo_pins[servo_count] = {0, 1, 2, 3}; // PCA9685 channels for each joint
const float l0 = 70;
const float l1 = 146.1;
const float l2 = 187.3;
const float l3 = 50;
// variables
int joint_angles[servo_count] = {90, 90, 90, 90}; // Initial angles for each joint (in degrees)

int degToMs(int degrees, int joint); */