// Header files
#include "moveTo.h"

// Variables to keep track of the current servo angles and target position
static float currentServoAngles[4];
static bool hasCurrentTarget = false;
static float currentReach;
static float currentHeight;
static float currentBaseAngle;

// Function to set the angle of a specific servo channel
void setServoAngle(uint8_t ch, float deg) {
    // Constrain angle to 0-180 degrees
    deg = constrain(deg, 0.0f, 180.0f);

    // Convert angle to pulse width in microseconds
    float us = PULSE_MIN_US + (deg / 180.0f) * (PULSE_MAX_US - PULSE_MIN_US);

    // Set the PWM signal for the servo channel
    pwm.writeMicroseconds(ch, (uint16_t)us);

    // Update the current servo angle for the channel
    if (ch < 4){
        currentServoAngles[ch] = deg;
    }
}


// Function to calculate the servo angles needed to reach a position
static bool checkServoAngles(float reachMm, float heightMm, float baseAngleDeg, float angles[4]) {

    // Check if the horizontal reach is negative
    if (reachMm < 0.0f) {
        Serial.println("Horizontal reach cannot be negative");
        return false;
    }

    // Use the inverse kinematics solver to calculate the joint angles
    JointAngles a;

    // Check if the target position is reachable
    if (!solver.solve(reachMm, heightMm, a)) {
        Serial.println("Target out of reach");
        return false;
    }

    // Calculate the servo angles based on the joint angles and offsets
    angles[0] = BASE_OFFSET + BASE_DIR * baseAngleDeg;
    angles[1] = OFFSET[0] + DIR[0] * (a.j1 - JOINT_REF[0]);
    angles[2] = OFFSET[1] + DIR[1] * (a.j2 - JOINT_REF[1]);
    angles[3] = OFFSET[2] + DIR[2] * (a.j3 - JOINT_REF[2]);

    // Check if the calculated angles are within the servo limits
    if (angles[0] < 0 || angles[0] > 180 ||
        angles[1] < 0 || angles[1] > 180 ||
        angles[2] < 0 || angles[2] > 180 ||
        angles[3] < 0 || angles[3] > 180) {
    Serial.println("Target exceeds servo limits");
    return false;
    }

    // If all checks pass, return true
    return true;
}

// Function to move the robot arm to a specified position with smooth interpolation
bool moveTo(float reachMm, float heightMm, float baseAngleDeg) {

    // Define the servo channels and check the target angles
    const uint8_t channels[] = {CH_BASE, CH_J1, CH_J2, CH_J3};
    float targets[4];
    if (!checkServoAngles(reachMm, heightMm, baseAngleDeg, targets)){
        return false;
    } 

    // If there is a current target, interpolate the movement to the new target
    if (hasCurrentTarget) {

        // Calculate the differences in reach, height, and base angle
        float dReach = reachMm - currentReach;
        float dHeight = heightMm - currentHeight;
        float dAngle = baseAngleDeg - currentBaseAngle;

        // Calculate the distance to the new target and determine the number of steps for interpolation
        float distance = sqrtf(dReach * dReach + dHeight * dHeight);
        int steps = (int)distance;

        // If the distance is not an integer, increment the number of steps
        if ((float)steps < distance) ++steps;
        
        // Calculate the number of steps needed for the base angle change
        int angleSteps = (int)fabsf(dAngle);

        // If the angle change is not an integer, increment the number of steps
        if ((float)angleSteps < fabsf(dAngle)) ++angleSteps;

        // Update the number of steps to be the maximum of the distance and angle steps
        if (angleSteps > steps) steps = angleSteps;

        // Ensure that there is at least one step for interpolation
        if (steps < 1) steps = 1;


    // // check that all intermediate positions are reachable
    // for (int step = 1; step <= steps; ++step) {
    //     // calcuate the interpolation factor t
    //     float t = (float)step / steps;
    //     // apply easing function for smooth move
    //     float eased = t * t * (3.0f - 2.0f * t);
    //     float intermediateAngles[4]; 

    //     // check if the intermediate position is reachable
    //     if (!checkServoAngles(
    //             currentReach + dReach * eased,
    //             currentHeight + dHeight * eased,
    //             currentBaseAngle + dAngle * eased,
    //             intermediateAngles)) {
    //         return false;
    //     }
    // }

    // 
    for (int step = 1; step <= steps; ++step) {
        float t = (float)step / steps;
        float eased = t * t * (3.0f - 2.0f * t);
        float intermediateAngles[4];
        if (!checkServoAngles(
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