#include "global.h"
#include "moveTo.h"
#include <math.h>

static float currentServoAngles[4];
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
      delay(10);
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
      delay(10);
    }
  }

  currentReach = reachMm;
  currentHeight = heightMm;
  currentBaseAngle = baseAngleDeg;
  hasCurrentTarget = true;
  return true;
}