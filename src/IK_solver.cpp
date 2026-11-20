/* Inverse Kinematics Solver for Lynx Motion AL5D Robot Arm with planar end effector*/

#include <math.h>
#include "IK_solver.h"

static const float RAD2DEG = 180.0f / 3.14159265358979f;

IKSolver::IKSolver(const ArmGeometry &geometry) : _g(geometry) {}

bool IKSolver::solve(float x, float y, JointAngles &out, bool elbowUp) const {
  // The end-effector link is horizontal, so the wrist (J3) sits L3 behind the target.
  float wx = x - _g.l3;
  float wy = y - _g.baseHeight;  // relative to J1

  float d2 = wx * wx + wy * wy;
  float c2 = (d2 - _g.l1 * _g.l1 - _g.l2 * _g.l2) / (2.0f * _g.l1 * _g.l2);

  if (c2 < -1.0f || c2 > 1.0f) return false;  // out of reach

  float th2 = elbowUp ? -acosf(c2) : acosf(c2);
  float th1 = atan2f(wy, wx) - atan2f(_g.l2 * sinf(th2), _g.l1 + _g.l2 * cosf(th2));

  // Keep link 3 parallel to the ground: th1 + th2 + th3 = 0
  float th3 = -(th1 + th2);

  out.j1 = th1 * RAD2DEG;
  out.j2 = th2 * RAD2DEG;
  out.j3 = th3 * RAD2DEG;
  return true;
}

bool IKSolver::solve3D(float x, float y, float z, JointAngles &out, bool elbowUp) const {
  float r = sqrtf(x * x + z * z);
  if (!solve(r, y, out, elbowUp)) return false;
  out.base = atan2f(z, x) * RAD2DEG;
  return true;
}