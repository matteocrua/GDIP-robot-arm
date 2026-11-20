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
    base height = 70,
    l1 (J1 - J2) = 146.1,
    l2 (J2 - J3) = 187.3,
    l3 (J3 - end effector) = 50

  1500us is the neutral position for all joints, and 600u is the minimum and 2400u is the maximum.
  
  The robots starting position will face the key holder, 
  and move to the right to place the key on the desk. 
  The robot will then return to its starting position.
*/

#include "main.h" // header file
#include <math.h>



void setServoAngle(uint8_t ch, float deg) {
  deg = constrain(deg, 0.0f, 180.0f);
  float us = PULSE_MIN_US + (deg / 180.0f) * (PULSE_MAX_US - PULSE_MIN_US);
  pwm.writeMicroseconds(ch, (uint16_t)us);
  if (ch < 4) currentServoAngles[ch] = deg;
}

bool moveTo(float x, float y, float z) {
  JointAngles a;
  if (!solver.solve3D(x, y, z, a)) {
    Serial.println("Target out of reach");
    return false;
  }

  float sb = BASE_OFFSET + BASE_DIR * a.base;
  float s1 = OFFSET[0] + DIR[0] * (a.j1 - JOINT_REF[0]);
  float s2 = OFFSET[1] + DIR[1] * (a.j2 - JOINT_REF[1]);
  float s3 = OFFSET[2] + DIR[2] * (a.j3 - JOINT_REF[2]);

  Serial.print("Servo angles -> B: "); Serial.print(sb, 1);
  Serial.print("  S1: "); Serial.print(s1, 1);
  Serial.print("  S2: "); Serial.print(s2, 1);
  Serial.print("  S3: "); Serial.println(s3, 1);

  if (sb < 0 || sb > 180 || s1 < 0 || s1 > 180 ||
      s2 < 0 || s2 > 180 || s3 < 0 || s3 > 180) {
    Serial.println("Target exceeds servo limits");
    return false;
  }

  const uint8_t channels[] = {CH_BASE, CH_J1, CH_J2, CH_J3};
  const float targets[] = {sb, s1, s2, s3};
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
    delay(20);
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("Enter target as: x y   (mm, e.g. 250 120)");
  setServoAngle(CH_BASE, BASE_OFFSET);
  setServoAngle(CH_J1, OFFSET[0]*DIR[0]);
  setServoAngle(CH_J2, OFFSET[1]*DIR[1]);
  setServoAngle(CH_J3, OFFSET[2]*DIR[2]);

  delay(1000); // wait for 1 second before moving to the target position
  moveTo(250, 120, 0); // move to the target position (x=250mm, y=120mm)
}

void loop() {
  if (Serial.available()) {
    float x = Serial.parseFloat();
    float y = Serial.parseFloat();
    float z = Serial.parseFloat();
    while (Serial.available()) Serial.read();
    moveTo(x, y, z);
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
