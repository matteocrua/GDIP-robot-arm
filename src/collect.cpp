#include "collect.h"

int offsetX = 70; // offset in mm to move away from the key location after collecting the key

// a set of functiona to collect keys from a predifined set of locations and return them to the user x, y, angle) in mm and degrees

// vertical array of predefined locations for the robot arm to collect keys from
float keyLocations[3][3] = {
  {200, 130, 90},   // location 1
  {200, 230, 90},   // location 2
  {200, 300, 90}    // location 3
};

void collectKey(int keyID){

    if (keyID < 1 || keyID > 3){
        Serial.println("Invalid key ID. Please enter a value between 1 and 3.");
        return;
    }

    // user types in serial terminal the key ID to collect, and the robot arm will move to the corresponding location and pick up the key
    Serial.println("Collecting key " + String(keyID + 1) + "...");

    keyID = keyID - 1; // convert to 0-based index

    // move towards key location
    moveTo(keyLocations[keyID][0] - offsetX, keyLocations[keyID][1], keyLocations[keyID][2]);
    delay(100); // wait for 1.5 seconds before moving to the target position

    // move into key location
    moveTo(keyLocations[keyID][0], keyLocations[keyID][1], keyLocations[keyID][2]);
    delay(2200); // wait for 1 second before moving to the target position

    // move away from key location
    moveTo(keyLocations[keyID][0] - offsetX, keyLocations[keyID][1], keyLocations[keyID][2]);
    delay(100); // wait for 3 seconds before moving to the target position

    // after collecting the key, the robot arm will return to its starting position
    Serial.println("Returning to starting position...");
    moveTo(250, 120, 0); // reach 250mm, height 120mm, base angle 0 degrees

}