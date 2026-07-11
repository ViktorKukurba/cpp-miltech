#include <fstream>
#include <iostream>
#include <ostream>
#include "mission_processor.hpp"

class FileConfigLoader : public IConfigLoader {
public:
  void load() override
  {
    ammoCount = loadAmmo();
    loadConfig();
  };
  DroneConfig getConfig() override { return config; };
  AmmoParams getAmmoParams() override
  {
    for (size_t i = 0; i < ammoCount; i++) {
      if (strcmp(ammos[i].name, config.ammoName) == 0) {
        return ammos[i];
      }
    }
    throw runtime_error("Ammo not found!");
  };

private:
  const char* CONFIG_FILE = "homework_07/data/config.json";
  const char* AMMO_FILE = "homework_07/data/ammo.json";
  AmmoParams* ammos = nullptr;
  DroneConfig config;
  u_int16_t ammoCount;

  u_int16_t loadAmmo()
  {
    ifstream fin(AMMO_FILE);
    if (!fin) {
      throw runtime_error("Could not open file: " + string(AMMO_FILE));
    }
    json j;
    fin >> j;
    u_int16_t count = j.size();
    ammos = new AmmoParams[count];
    for (int i = 0; i < count; i++) {
      std::strncpy(ammos[i].name, j[i]["name"].get<string>().c_str(), 31);
      ammos[i].mass = j[i]["mass"];
      ammos[i].drag = j[i]["drag"];
      ammos[i].lift = j[i]["lift"];
    }
    return count;
  }

  void loadConfig()
  {
    ifstream fin(CONFIG_FILE);
    if (!fin) {
      throw runtime_error("Could not open file: " + string(CONFIG_FILE));
    }
    json j;
    fin >> j;

    // Доступ до полів
    config.startPos.x = j["drone"]["position"]["x"];
    config.startPos.y = j["drone"]["position"]["y"];
    config.altitude = j["drone"]["altitude"];
    config.accelPath = j["drone"]["accelerationPath"];
    config.angularSpeed = j["drone"]["angularSpeed"];
    config.attackSpeed = j["drone"]["attackSpeed"];
    config.initialDir = j["drone"]["initialDirection"];
    config.turnThreshold = j["drone"]["turnThreshold"];

    config.simTimeStep = j["simulation"]["timeStep"];
    config.hitRadius = j["simulation"]["hitRadius"];
    config.arrayTimeStep = j["targetArrayTimeStep"];

    string s = j["ammo"];
    strncpy(config.ammoName, s.c_str(), 31);
  }
};

class JsonTargetProvider : public ITargetProvider {
public:
  JsonTargetProvider(const char* file) { loadTargets(file); }
  int getTargetCount() override { return targetCount; };
  Coord* getTarget(int index) override { return targets[index]; };
  Coord getTargetCoord(int index, float) override { return targets[index][0]; };
  int getTimeSteps() override { return timeSteps; };

private:
  Coord** targets;
  int targetCount;
  int timeSteps;
  void loadTargets(const char* file)
  {
    ifstream ft(file);
    if (!ft) {
      throw runtime_error("Could not open file: " + string(file));
    }
    json jt;
    ft >> jt;
    targetCount = jt["targetCount"];
    timeSteps = jt["timeSteps"];

    targets = new Coord*[targetCount];
    for (int i = 0; i < targetCount; i++) {
      targets[i] = new Coord[timeSteps];
      for (int j = 0; j < timeSteps; j++) {
        targets[i][j].x = jt["targets"][i]["positions"][j]["x"];
        targets[i][j].y = jt["targets"][i]["positions"][j]["y"];
      }
    }
  }
};

class AnalyticalSolver : public IBallisticSolver {
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

IConfigLoader* createLoader(LoaderType type)
{
  switch (type) {
    case LoaderType::FILE:
      return new FileConfigLoader();
  }
};

ITargetProvider* createProvider(ProviderType type, const char* param)
{
  switch (type) {
    case ProviderType::JSON:
      return new JsonTargetProvider(param);
  }
};

IBallisticSolver* createSolver(SolverType type)
{
  switch (type) {
    case SolverType::ANALYTICAL:
      return new AnalyticalSolver;
  }
};

class MissionProcessor {
  IBallisticSolver* solver;  // стратегія
  ITargetProvider* targets;

public:
  MissionProcessor(IBallisticSolver* s, ITargetProvider* t)
    : solver(s)
    , targets(t)
  {
  }

  ~MissionProcessor() { delete[] steps; }

  void init(IConfigLoader* configSource)
  {
    configSource->load();
    ammo = configSource->getAmmoParams();
    droneConfig = configSource->getConfig();
  }

  SimStep* simulation()
  {
    steps[0] = SimStep{.pos = droneConfig.startPos, .direction = droneConfig.initialDir, .state = DroneState::STOPPED};

    stepCount++;

    while (hasNext()) {
      steps[stepCount] = step();
      cout << "simulation";
      LOG_Step(steps[stepCount], stepCount);
      if (canHit(droneConfig, steps[stepCount])) {
        break;
      }

      stepCount++;
    }

    return steps;
  }

  SimStep step()
  {
    const int targetsCount = targets->getTargetCount();
    cout << "step " << stepCount << endl;
    SimStep simStep = steps[stepCount - 1];
    SimStep newSimStep =
      SimStep{.pos = simStep.pos, .direction = simStep.direction, .state = simStep.state, .targetIdx = simStep.targetIdx};

    vector<SolverResult> results(targetsCount);
    uint8_t selectedTargetIdx = 0;
    for (size_t i = 0; i < targetsCount; i++) {
      int idx = (int)floor(currentTime / droneConfig.arrayTimeStep) % targets->getTimeSteps();
      const Coord targetPos = targets->getTargetCoord(i, idx);
      const Coord targetPrevPos = targets->getTargetCoord(i, idx - 1);
      SolverResult res = solver->solve(droneConfig, newSimStep.pos, targetPos, targetPrevPos, ammo, targets->getTimeSteps());
      results[i] = res;
      cout << "Time:" << res.timeToPos << endl;
      if (results[selectedTargetIdx].timeToPos > res.timeToPos) {
        selectedTargetIdx = i;
      }
    }

    newSimStep.predictedTarget = results[selectedTargetIdx].interpolatedTargetPos;
    newSimStep.dropPoint = results[selectedTargetIdx].balisticPoint;
    newSimStep.targetIdx = selectedTargetIdx;

    float theta = getTheta(newSimStep.pos, newSimStep.predictedTarget);
    double delta = getDelta(theta, newSimStep.direction);
    float speed = getCurrentSpeed(steps, stepCount, droneConfig);

    if ((delta > droneConfig.turnThreshold) && (speed > 0)) {
      decelerateDrone(droneConfig, newSimStep, speed);
    }
    else if ((delta > droneConfig.turnThreshold) && (newSimStep.state == DroneState::STOPPED || newSimStep.state == DroneState::TURNING)) {
      turnDrone(droneConfig, newSimStep, theta);
    }
    else if (speed < droneConfig.attackSpeed) {
      acelerateDrone(droneConfig, newSimStep, speed);
    }
    else {
      moveDrone(droneConfig, newSimStep);
    }

    currentTime += droneConfig.simTimeStep;
    return newSimStep;
  }

  void reset() { stepCount = 0; }

  void changeSolver(IBallisticSolver* s) { solver = s; }

  bool hasNext() { return stepCount < MAX_STEPS; }  // MAX_STEPS

  int getStepsCount() { return stepCount; }

private:
  int stepCount = 0;
  AmmoParams ammo;
  DroneConfig droneConfig;
  float currentTime = 0;
  SimStep* steps = new SimStep[MAX_STEPS];

  float getTheta(const Coord& a, const Coord& b)
  {
    Coord delta = b - a;
    return atan2(delta.y, delta.x);
  }

  float getDelta(float thetaA, float thetaB) { return abs(atan2(sin(thetaA - thetaB), cos(thetaA - thetaB))); }

  float normalizeTheta(float theta) { return abs(atan2(sin(theta), cos(theta))); }

  float calculateTurn(float currentTheta, float targetTheta, float angularSpeed, float dt)
  {
    float delta = targetTheta - currentTheta;

    // Normalize
    while (delta > M_PI)
      delta -= 2 * M_PI;
    while (delta < -M_PI)
      delta += 2 * M_PI;

    float turnStep = angularSpeed * dt;
    if (abs(delta) > turnStep) {
      delta = (delta > 0 ? 1 : -1) * turnStep;
    }
    LOG("TURN: " + to_string(currentTheta) + " -> " + to_string(targetTheta) + " res: " + to_string(currentTheta + delta));
    currentTheta += delta;
    return currentTheta;
  }

  // ===

  // === LOG functions
  void LOG_Coord(const Coord& c) { LOG("Coords: x=" + to_string(c.x) + "; y=" + to_string(c.y)); }

  void LOG_Step(const SimStep& step, int idx)
  {
    LOG("Simulation step: " + to_string(idx));
    LOG_Coord(step.pos);
    LOG("Direction: " + to_string(step.direction));
    LOG("State: " + to_string(step.state));
    LOG("Target: " + to_string(step.targetIdx));
  }

  void decelerateDrone(const DroneConfig& config, SimStep& step, float speed)
  {
    step.state = DroneState::DECELERATING;

    const float a = -config.attackSpeed * config.attackSpeed / (2 * config.accelPath);
    speed = speed + a * config.simTimeStep;
    if (speed <= 0) {
      speed = 0;
      step.state = DroneState::STOPPED;
    }
    step.pos = step.pos.move(step.direction, config.simTimeStep, speed);
  }

  void turnDrone(const DroneConfig& config, SimStep& step, float theta)
  {
    step.state = DroneState::TURNING;
    step.direction = normalizeTheta(calculateTurn(step.direction, theta, config.angularSpeed, config.simTimeStep));
  }

  void acelerateDrone(const DroneConfig& config, SimStep& step, float speed)
  {
    step.state = DroneState::ACCELERATING;
    const float a = config.attackSpeed * config.attackSpeed / (2 * config.accelPath);
    speed = speed + a * config.simTimeStep;
    if (speed >= config.attackSpeed) {
      speed = config.attackSpeed;
      step.state = DroneState::MOVING;
    }
    step.pos = step.pos.move(step.direction, config.simTimeStep, speed);
  }

  void moveDrone(const DroneConfig& config, SimStep& step)
  {
    step.state = DroneState::MOVING;
    step.pos = step.pos.move(step.direction, config.simTimeStep, config.attackSpeed);
  }

  float getCurrentSpeed(SimStep* steps, int step, const DroneConfig& config)
  {
    if (step < 2) {
      return 0;
    }
    const float speed = Coord::getDistance(steps[step - 1].pos, steps[step - 2].pos) / config.simTimeStep;
    return speed;
  }

  bool canHit(const DroneConfig& config, const SimStep& step)
  {
    float d = Coord::getDistance(step.pos, step.dropPoint);
    return d < config.hitRadius && step.state == DroneState::MOVING;
  }
};
