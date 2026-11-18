/* 
  GDIP key fetcher project using lynx motion AL5D robot arm, 
  with PCA9685 and an arduino uno.
  
  This robot will use an AI model to interact with the user and take commands 
  to move the robot arm to fetch a key from a key holder.

  The robot arm has 4 joints;
    joint 0: base
    joint 1: lower arm
    joint 2: upper arm
    joint 3: planar end effector with electromagnet

  Joint lengths:
    l1 (J0-J1) = 145,
    l2 (J1-J2) = 215,
    l3 (J2-J3) = 100,

  The robots starting position will face the key holder, 
  and move to the right to place the key on the desk. 
  The robot will then return to its starting position.
*/





#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}