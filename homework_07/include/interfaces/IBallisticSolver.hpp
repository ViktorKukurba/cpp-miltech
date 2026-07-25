#pragma once

#include "types.hpp"

struct SolverResult {
  Coord interpolatedTargetPos;
  Coord balisticPoint;
  float timeToPos;
};

class IBallisticSolver {
public:
  virtual SolverResult solve(const DroneConfig& config, const Coord& dronePos, const Target& target, AmmoParams ammo) = 0;
  virtual ~IBallisticSolver() = default;
};
