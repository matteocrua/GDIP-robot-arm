/* 
  GDIP key fetcher project using lynx motion AL5D robot arm, 
  with PCA9685 and an arduino uno.
  
  This robot will use an AI model to interact with the user and take commands 
  to move the robot arm to fetch a key from a key holder.

  The robot arm has 5 joints;
    joint 0: base
    joint 1: shoulder
    joint 2: elbow
    joint 3: wrist

  Joint lengths:
    base height = 90,
    l1 (J1 - J2) = 147,
    l2 (J2 - J3) = 182.5,
    l3 (J3 - end effector) = 50

  1500us is the neutral position for all joints, and 600u is the minimum and 2400u is the maximum.
  
  The robots starting position will face the key holder, 
  and move to the right to place the key on the desk. 
  The robot will then return to its starting position.
*/

#include "main.h" // header file
#include <math.h>

static bool hasCurrentTarget = false;
static float currentReach;
static float currentHeight;
static float currentBaseAngle;

void setServoAngle(uint8_t ch, float deg) {
  deg = constrain(deg, 0.0f, 180.0f);
  float us = PULSE_MIN_US + (deg / 180.0f) * (PULSE_MAX_US - PULSE_MIN_US);
  pwm.writeMicroseconds(ch, (uint16_t)us);
  if (ch < 4) currentServoAngles[ch] = deg;
}

static bool calculateServoAngles(float reachMm, float heightMm, float baseAngleDeg, float angles[4]) {
  if (reachMm < 0.0f) {
    Serial.println("Horizontal reach cannot be negative");
    return false;
  }

  JointAngles a;
  if (!solver.solve(reachMm, heightMm, a)) {
    Serial.println("Target out of reach");
    return false;
  }

  angles[0] = BASE_OFFSET + BASE_DIR * baseAngleDeg;
  angles[1] = OFFSET[0] + DIR[0] * (a.j1 - JOINT_REF[0]);
  angles[2] = OFFSET[1] + DIR[1] * (a.j2 - JOINT_REF[1]);
  angles[3] = OFFSET[2] + DIR[2] * (a.j3 - JOINT_REF[2]);

  if (angles[0] < 0 || angles[0] > 180 ||
      angles[1] < 0 || angles[1] > 180 ||
      angles[2] < 0 || angles[2] > 180 ||
      angles[3] < 0 || angles[3] > 180) {
    Serial.println("Target exceeds servo limits");
    return false;
  }

  return true;
}

bool moveTo(float reachMm, float heightMm, float baseAngleDeg) {
  const uint8_t channels[] = {CH_BASE, CH_J1, CH_J2, CH_J3};
  float targets[4];
  if (!calculateServoAngles(reachMm, heightMm, baseAngleDeg, targets)) return false;

  Serial.print("Servo angles -> B: "); Serial.print(targets[0], 1);
  Serial.print("  S1: "); Serial.print(targets[1], 1);
  Serial.print("  S2: "); Serial.print(targets[2], 1);
  Serial.print("  S3: "); Serial.println(targets[3], 1);

  if (hasCurrentTarget) {
    float dReach = reachMm - currentReach;
    float dHeight = heightMm - currentHeight;
    float dAngle = baseAngleDeg - currentBaseAngle;
    float distance = sqrtf(dReach * dReach + dHeight * dHeight);
    int steps = (int)distance;
    if ((float)steps < distance) ++steps;
    int angleSteps = (int)fabsf(dAngle);
    if ((float)angleSteps < fabsf(dAngle)) ++angleSteps;
    if (angleSteps > steps) steps = angleSteps;
    if (steps < 1) steps = 1;

    for (int step = 1; step <= steps; ++step) {
      float t = (float)step / steps;
      float eased = t * t * (3.0f - 2.0f * t);
      float intermediateAngles[4];
      if (!calculateServoAngles(
              currentReach + dReach * eased,
              currentHeight + dHeight * eased,
              currentBaseAngle + dAngle * eased,
              intermediateAngles)) {
        return false;
      }
    }

    for (int step = 1; step <= steps; ++step) {
      float t = (float)step / steps;
      float eased = t * t * (3.0f - 2.0f * t);
      float intermediateAngles[4];
      if (!calculateServoAngles(
              currentReach + dReach * eased,
              currentHeight + dHeight * eased,
              currentBaseAngle + dAngle * eased,
              intermediateAngles)) {
        return false;
      }
      for (uint8_t i = 0; i < 4; ++i) {
        setServoAngle(channels[i], intermediateAngles[i]);
      }
      delay(5);
    }
  } else {
    float startAngles[4];
    float maxDelta = 0.0f;
    for (uint8_t i = 0; i < 4; ++i) {
      startAngles[i] = currentServoAngles[channels[i]];
      float delta = fabsf(targets[i] - startAngles[i]);
      if (delta > maxDelta) maxDelta = delta;
    }

    int steps = (int)maxDelta;
    if ((float)steps < maxDelta) ++steps;
    if (steps < 1) steps = 1;

    for (int step = 1; step <= steps; ++step) {
      float t = (float)step / steps;
      float eased = t * t * (3.0f - 2.0f * t);
      for (uint8_t i = 0; i < 4; ++i) {
        float angle = startAngles[i] + (targets[i] - startAngles[i]) * eased;
        setServoAngle(channels[i], angle);
      }
      delay(5);
    }
  }

  currentReach = reachMm;
  currentHeight = heightMm;
  currentBaseAngle = baseAngleDeg;
  hasCurrentTarget = true;
  return true;
}

void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("Enter target as: reach height base-angle (mm, degrees; e.g. 200 150 45)");
  setServoAngle(CH_BASE, BASE_OFFSET);
  setServoAngle(CH_J1, OFFSET[0]*DIR[0]);
  setServoAngle(CH_J2, OFFSET[1]*DIR[1]);
  setServoAngle(CH_J3, OFFSET[2]*DIR[2]);

  //delay(1000); // wait for 1 second before moving to the target position
  //moveTo(250, 120, 0); // reach 250mm, height 120mm, base angle 0 degrees
}

void loop() {

  if (Serial.available()) {
    float reachMm = Serial.parseFloat();
    float heightMm = Serial.parseFloat();
    float baseAngleDeg = Serial.parseFloat();
    while (Serial.available()) Serial.read();
    moveTo(reachMm, heightMm, baseAngleDeg);

  }
}









/* void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);
  
  
}

void loop() {


} */
