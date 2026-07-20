#pragma once

#include <cmath>
#include <stdexcept>

#include "solvers/AnalyticalSolver.hpp"

class AnalyticalSolver : public IAnalyticalSolver {
public:
  SolverResult solve(const DroneConfig& config,
                     const Coord& dronePos,
                     const Coord& targetPos,
                     const Coord& targetPrevPos,
                     AmmoParams ammo,
                     int timeStepSize) override
  {
    const float t = getTotalTimeToPos(config, dronePos, targetPos);
    const Coord iP = getInterpolatedTargetPos(config, targetPos, targetPrevPos, t, timeStepSize);
    const Coord bP = getBalisticPoint(config, dronePos, iP, ammo);
    const float tB = getTotalTimeToPos(config, dronePos, bP);
    return SolverResult{.interpolatedTargetPos = iP, .balisticPoint = bP, .timeToPos = tB};
  }

private:
  Coord getInterpolatedTargetPos(const DroneConfig& config, const Coord& target, const Coord& targetPrevPos, float t, int timeStepSize)
  {
    int idx = (int)floor(t / config.arrayTimeStep) % timeStepSize;
    float frac = (t - idx * config.arrayTimeStep) / config.arrayTimeStep;
    return target + (target - targetPrevPos) * frac;
  }

  float getTotalTimeToPos(const DroneConfig& config, const Coord& dronePos, const Coord& target)
  {
    const double tD = Coord::getDistance(dronePos, target);
    const float t = tD / config.attackSpeed;
    return t;
  }

  Coord getBalisticPoint(const DroneConfig& config, const Coord& drone, const Coord& target, const AmmoParams& ammo)
  {
    float h = getHorizontalFlightDistance(ammo, config);

    float D = Coord::getDistance(drone, target);
    Coord iP;
    bool hasIntermediate = h + config.accelPath > D;

    if (hasIntermediate) {
      iP = getIntermediatePoint(config, drone, target, h, D);
    }

    Coord p = getPoint(drone, target, h, D);

    return p;
  }

  double calculateTimeOfFlight(const AmmoParams& aD, const DroneConfig& config)
  {
    double m = aD.mass;
    double m2 = pow(m, 2);
    double l = aD.lift;
    double d = aD.drag;
    double d2 = pow(d, 2);
    const double V0 = config.attackSpeed;

    double a = d * G * m - 2 * d2 * l * V0;
    double b = -3 * G * m2 + 3 * d * l * m * V0;
    double c = 6 * m2 * config.altitude;

    double p = -pow(b, 2) / (3 * pow(a, 2));
    double q = 2 * pow(b, 3) / (27 * pow(a, 3)) + c / a;

    double phi = acos(3 * q / (2 * p) * sqrt(-3 / p));

    double t = 2 * sqrt(-p / 3) * cos((phi + 4 * M_PI) / 3) - b / (3 * a);

    return t;
  }

  double getHorizontalFlightDistance(const AmmoParams& aD, const DroneConfig& config)
  {
    const double V0 = config.attackSpeed;
    const double d = aD.drag;
    const double l = aD.lift;
    const double m = aD.mass;
    const double d2 = pow(d, 2);
    const double d3 = pow(d, 3);
    const double d4 = pow(d, 4);
    const double l2 = pow(l, 2);
    const double l3 = pow(l, 3);
    const double l4 = pow(l, 4);
    const double m2 = pow(m, 2);
    const double m3 = pow(m, 3);
    const double m4 = pow(m, 4);
    double t = calculateTimeOfFlight(aD, config);
    if (t <= 0 || isnan(t)) {
      string error = "Not able to calculate Time of Flight correctly for current input";
      throw runtime_error(error);
    }
    const double t2 = pow(t, 2);
    const double t3 = pow(t, 3);
    const double t4 = pow(t, 4);
    const double t5 = pow(t, 5);

    double D = V0 * t - t2 * d * V0 / (2 * m) + t3 * (6 * d * G * l * m - 6 * d2 * (l2 - 1) * V0) / (36 * m2) +
               t4 * (-6 * d2 * G * l * (1 + l2 + l4) * m + 3 * d3 * l2 * (1 + l2) * V0 + 6 * d3 * l4 * (1 + l2) * V0) /
                 (36 * pow((1 + l2), 2) * m3) +
               t5 * (3 * d3 * G * l3 * m - 3 * d4 * l2 * (1 + l2) * V0) / (36 * (1 + l2) * m4);

    if (D <= 0 || isnan(t)) {
      string error = "Not able to calculate Horizontal Flight Distance correctly for current input";
      throw runtime_error(error);
    }

    return D;
  }

  Coord getPoint(Coord drone, Coord target, double h, double D)
  {
    float ratio = (D - h) / D;

    return drone + (target - drone) * ratio;
  };

  Coord getXMoveFromTarget(const DroneConfig& config, const Coord& drone, float h)
  {
    return Coord{drone.x - (h + config.accelPath), drone.y};
  }

  Coord getIntermediatePoint(const DroneConfig& config, const Coord& drone, const Coord& target, float horDis, float D)
  {
    if (D == 0) {
      return getXMoveFromTarget(config, drone, horDis);
    }
    else {
      double c = (horDis + config.accelPath) / D;
      return target - (target - drone) * c;
    }
  }
};