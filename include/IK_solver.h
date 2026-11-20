#ifndef IK_SOLVER_H
#define IK_SOLVER_H

// Result of the IK solve, in degrees.
//   j1: angle of link 1 above horizontal (0 = forward, 90 = straight up)
//   j2: bend of link 2 relative to link 1 (negative = elbow up)
//   j3: bend of link 3 relative to link 2 (keeps link 3 horizontal)
struct JointAngles {
    float base;  // angle of base rotation (0 = forward, 90 = left)
    float j1;
    float j2;
    float j3;
};

// Arm dimensions in mm.
struct ArmGeometry {
    float baseHeight;  // ground to J1 axis
    float l1;          // J1 - J2
    float l2;          // J2 - J3
    float l3;          // J3 - end effector (kept horizontal)
};

class IKSolver {
    public:
        explicit IKSolver(const ArmGeometry &geometry);

        // x = horizontal distance from J1 axis (mm), y = height above ground (mm).
        // Returns false if the target is out of reach (angles left untouched).
        bool solve(float x, float y, JointAngles &out, bool elbowUp = true) const;
        bool solve3D(float x, float y, float z, JointAngles &out, bool elbowUp = true) const;


    private:
        ArmGeometry _g;
};



#endif // IK_SOLVER_H