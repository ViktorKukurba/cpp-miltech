#include <cmath>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include "interfaces/ITargetProvider.hpp"
#include "interfaces/states/StateStopped.hpp"
#include "MissionProcessor.hpp"
#include "utils.hpp"

class MissionProcessor : public IMissionProcessor {
private:
  std::unique_ptr<IBallisticSolver> solver;  // стратегія
  std::unique_ptr<ITargetProvider> targets;

public:
  MissionProcessor(std::unique_ptr<IBallisticSolver> s, std::unique_ptr<ITargetProvider> t)
    : solver(std::move(s))
    , targets(std::move(t))
  {
  }

  void init(unique_ptr<IConfigLoader> configSource) override
  {
    configSource->load();
    ammo = configSource->getAmmoParams();
    droneConfig = configSource->getConfig();
  }

  vector<SimStep> simulation() override
  {
    steps[0] = SimStep{.pos = droneConfig.startPos, .direction = droneConfig.initialDir, .state = DroneState::STOPPED};

    stepCount++;

    state = std::make_unique<StateStopped>();

    while (hasNext()) {
      steps[stepCount] = step();
      LOG_Step(steps[stepCount], stepCount);
      if (canHit(droneConfig, steps[stepCount])) {
        break;
      }

      stepCount++;
    }

    return steps;
  }

  SimStep step() override
  {
    const int targetsCount = targets->getTargetCount();
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
      results.at(i) = res;
      cout << "Time:" << res.timeToPos << endl;
      if (results.at(selectedTargetIdx).timeToPos > res.timeToPos) {
        selectedTargetIdx = i;
      }
    }

    newSimStep.predictedTarget = results.at(selectedTargetIdx).interpolatedTargetPos;
    newSimStep.dropPoint = results.at(selectedTargetIdx).balisticPoint;
    newSimStep.targetIdx = selectedTargetIdx;

    float theta = getTheta(newSimStep.pos, newSimStep.predictedTarget);
    double delta = getDelta(theta, newSimStep.direction);
    float speed = getCurrentSpeed(steps, stepCount, droneConfig);

    DroneContext ctx({.desiredDir = theta, .direction = newSimStep.direction, .speed = speed, .cfg = droneConfig});
    auto next = state->execute(ctx);

    if (next)
      state = std::move(next);

    newSimStep.direction = ctx.direction;
    newSimStep.pos = newSimStep.pos.move(ctx.direction, droneConfig.simTimeStep, ctx.speed);
    newSimStep.state = state->type();

    currentTime += droneConfig.simTimeStep;
    return newSimStep;
  }

  void reset() override { stepCount = 0; }

  void changeSolver(unique_ptr<IBallisticSolver> s) override { solver = std::move(s); }

  bool hasNext() override { return stepCount < MAX_STEPS; }  // MAX_STEPS

  int getStepsCount() override { return stepCount; }

private:
  int stepCount = 0;
  AmmoParams ammo;
  DroneConfig droneConfig;
  float currentTime = 0;
  vector<SimStep> steps = vector<SimStep>(MAX_STEPS);
  std::unique_ptr<IDroneState> state;

  float getTheta(const Coord& a, const Coord& b)
  {
    Coord delta = b - a;
    return atan2(delta.y, delta.x);
  }

  float getDelta(float thetaA, float thetaB) { return abs(atan2(sin(thetaA - thetaB), cos(thetaA - thetaB))); }

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
    step.direction = Utils::normalizeAngle(calculateTurn(step.direction, theta, config.angularSpeed, config.simTimeStep));
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

  float getCurrentSpeed(vector<SimStep> steps, int step, const DroneConfig& config)
  {
    if (step < 2) {
      return 0;
    }
    const float speed = Coord::getDistance(steps.at(step - 1).pos, steps.at(step - 2).pos) / config.simTimeStep;
    return speed;
  }

  bool canHit(const DroneConfig& config, const SimStep& step)
  {
    float d = Coord::getDistance(step.pos, step.dropPoint);
    return d < config.hitRadius && step.state == DroneState::MOVING;
  }
};
