#pragma once

#include "types.hpp"

struct SolverResult {
  Coord interpolatedTargetPos;
  Coord balisticPoint;
  float timeToPos;
};

class IBallisticSolver {
public:
  virtual SolverResult solve(const DroneConfig& config,
                             const Coord& dronePos,
                             const Coord& targetPos,
                             const Coord& targetPrevPos,
                             AmmoParams ammo,
                             int timeStepSize) = 0;
  virtual ~IBallisticSolver() = default;
};
