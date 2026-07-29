#include "solvers/TableSolver.hpp"
#include "BallisticTable.hpp"
#include "types.hpp"

class TableSolver : public ITableSolver {
public:
  TableSolver() { table.load("homework_07/data/ballistic_table.txt"); }
  SolverResult solve(const DroneConfig& config, const Coord& dronePos, const Target& target, AmmoParams ammo) override
  {
    const float t = getTotalTimeToPos(config, dronePos, target.pos);
    const Coord iP = getInterpolatedTargetPos(target, t);
    const Coord bP = getBalisticPoint(config, dronePos, iP, ammo);
    const float tB = getTotalTimeToPos(config, dronePos, bP);
    return SolverResult{.interpolatedTargetPos = iP, .balisticPoint = bP, .timeToPos = tB};
  }

private:
  BallisticTable table;
  Coord getInterpolatedTargetPos(const Target& target, float t) { return target.pos + target.velocity * t; }

  float getTotalTimeToPos(const DroneConfig& config, const Coord& dronePos, const Coord& target)
  {
    const double tD = Coord::getDistance(dronePos, target);
    const float t = tD / config.attackSpeed;
    return t;
  }

  Coord getBalisticPoint(const DroneConfig& config, const Coord& drone, const Coord& target, const AmmoParams& ammo)
  {
    auto r = table.lookup(config.altitude, config.attackSpeed, ammo.mass, ammo.drag, ammo.lift);
    float h = r.hDist;

    float D = Coord::getDistance(drone, target);
    Coord iP;
    bool hasIntermediate = h + config.accelPath > D;

    if (hasIntermediate) {
      iP = getIntermediatePoint(config, drone, target, h, D);
    }

    Coord p = getPoint(drone, target, h, D);

    return p;
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
