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

// Header files
#include "main.h"

void setup() {
  // set baud rate for serial communication
  Serial.begin(115200);

  // initialize the PWM driver and set the frequency for the servos
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10); // wait for the PWM driver to initialize
  
  // set the initial position of the robot arm to the starting position
  setServoAngle(CH_BASE, BASE_OFFSET);
  setServoAngle(CH_J1, OFFSET[0]*DIR[0]);
  setServoAngle(CH_J2, OFFSET[1]*DIR[1]);
  setServoAngle(CH_J3, OFFSET[2]*DIR[2]);

  delay(1000); // wait for 1s
  moveTo(250, 120, 0); // face user starting position
  
}

void loop() {
  if (Serial.available()) {

    // prompt the user to enter a key ID to collect
    Serial.println("What key would you like to collect? (1, 2, or 3)");

    // read the key ID from the serial input
    int keyID = Serial.parseInt();

    // call collectKey function with the keyID
    collectKey(keyID);

    delay(10); // wait for 10ms
    Serial.println("Key collected successfully.");
  }
}