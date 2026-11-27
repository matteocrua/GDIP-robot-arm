#include "collect.h"

int offsetX = 70; // offset in mm to move away from the key location after collecting the key

// predefined key locations: {reach mm, height mm, base angle deg}
float keyLocations[3][3] = {
  {400, 130, 90},   // key 1
  {400, 230, 90},   // key 2
  {400, 300, 90}    // key 3
};

// Returns true if the whole sequence succeeded, false if any move failed
bool collectKey(int keyID) {

    if (keyID < 1 || keyID > 3) {
        Serial.println("Invalid key ID");
        return false;
    }

    Serial.println("Collecting key " + String(keyID) + "...");

    int i = keyID - 1; // convert to 0-based index

    // move towards key location
    if (!moveTo(keyLocations[i][0] - offsetX, keyLocations[i][1], keyLocations[i][2])) return false;
    delay(100);

    // move into key location
    if (!moveTo(keyLocations[i][0], keyLocations[i][1], keyLocations[i][2])) return false;
    delay(2200);

    // move away from key location
    if (!moveTo(keyLocations[i][0] - offsetX, keyLocations[i][1], keyLocations[i][2])) return false;
    delay(100);

    // return to starting position
    Serial.println("Returning to starting position...");
    if (!moveTo(250, 120, 0)) return false;

    return true;
}   