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
#include <stdlib.h>
#include <string.h>

bool moveTo(float x, float y, float z);

static bool parseCoordinate(const char *token, float &value) {
  char *end;
  double parsed = strtod(token, &end);
  if (end == token || *end != '\0' || !isfinite(parsed)) return false;
  value = (float)parsed;
  return true;
}

static void processSerialCommand(char *line) {
  char *token = strtok(line, " \t");
  if (token == NULL) return;

  bool hasMoveTo = strcmp(token, "moveto") == 0;
  if (hasMoveTo) {
    token = strtok(NULL, " \t");
  }

  float x, y, z;
  if (token == NULL || !parseCoordinate(token, x)) {
    Serial.println("Usage: moveto <x> <y> <z> (mm)");
    return;
  }

  token = strtok(NULL, " \t");
  if (token == NULL || !parseCoordinate(token, y)) {
    Serial.println("Usage: moveto <x> <y> <z> (mm)");
    return;
  }

  token = strtok(NULL, " \t");
  if (token == NULL || !parseCoordinate(token, z) ||
      strtok(NULL, " \t") != NULL) {
    Serial.println("Usage: moveto <x> <y> <z> (mm)");
    return;
  }

  moveTo(x, y, z);
}

void setServoAngle(uint8_t ch, float deg) {
  deg = constrain(deg, 0.0f, 180.0f);
  float us = PULSE_MIN_US + (deg / 180.0f) * (PULSE_MAX_US - PULSE_MIN_US);
  pwm.writeMicroseconds(ch, (uint16_t)us);
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

  setServoAngle(CH_BASE, sb);
  setServoAngle(CH_J1, s1);
  setServoAngle(CH_J2, s2);
  setServoAngle(CH_J3, s3);
  return true;
}

void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("Enter: moveto <x> <y> <z> (mm, e.g. moveto 250 120 0)");
  setServoAngle(CH_J1, OFFSET[0]*DIR[0]);
  setServoAngle(CH_J2, OFFSET[1]*DIR[1]);
  setServoAngle(CH_J3, OFFSET[2]*DIR[2]);

  delay(1000); // wait for 1 second before moving to the target position
  moveTo(250, 120, 0); // move to the target position (x=250mm, y=120mm)
}

void loop() {
  static char line[64];
  static uint8_t lineLength = 0;
  static bool lineTooLong = false;

  while (Serial.available()) {
    char character = (char)Serial.read();
    if (character == '\r') continue;

    if (character == '\n') {
      if (lineTooLong) {
        Serial.println("Command too long");
      } else {
        line[lineLength] = '\0';
        processSerialCommand(line);
      }
      lineLength = 0;
      lineTooLong = false;
    } else if (!lineTooLong) {
      if (lineLength < sizeof(line) - 1) {
        line[lineLength++] = character;
      } else {
        lineTooLong = true;
      }
    }
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
