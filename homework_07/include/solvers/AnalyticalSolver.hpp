#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
#include "interfaces/IBallisticSolver.hpp"

class IAnalyticalSolver : public IBallisticSolver {};

const double G = 9.81;

// class AnalyticalSolver : public IAnalyticalSolver {
// public:
//   SolverResult solve(const DroneConfig& config,
//                      const Coord& dronePos,
//                      const Coord& targetPos,
//                      const Coord& targetPrevPos,
//                      AmmoParams ammo,
//                      int timeStepSize) override;

// private:
//   Coord getInterpolatedTargetPos(const DroneConfig& config, const Coord& target, const Coord& targetPrevPos, float t, int timeStepSize);
//   float getTotalTimeToPos(const DroneConfig& config, const Coord& dronePos, const Coord& target);
//   Coord getBalisticPoint(const DroneConfig& config, const Coord& drone, const Coord& target, const AmmoParams& ammo);
//   double calculateTimeOfFlight(const AmmoParams& aD, const DroneConfig& config);
//   double getHorizontalFlightDistance(const AmmoParams& aD, const DroneConfig& config);
//   Coord getPoint(Coord drone, Coord target, double h, double D);
//   Coord getXMoveFromTarget(const DroneConfig& config, const Coord& drone, float h);
//   Coord getIntermediatePoint(const DroneConfig& config, const Coord& drone, const Coord& target, float horDis, float D);
// };