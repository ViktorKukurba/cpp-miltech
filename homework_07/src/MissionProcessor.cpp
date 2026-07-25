#include <cmath>
#include <memory>
#include <thread>
#include <vector>
#include "interfaces/IDronePhysics.hpp"
#include "interfaces/ITargetProvider.hpp"
#include "interfaces/states/StateStopped.hpp"
#include "MissionProcessor.hpp"
#include "utils.hpp"

class MissionProcessor : public IMissionProcessor {
private:
  unique_ptr<IBallisticSolver> solver;  // стратегія
  unique_ptr<ITargetProvider> targets;
  unique_ptr<IDronePhysics> drone;

public:
  MissionProcessor(std::unique_ptr<IBallisticSolver> s, std::unique_ptr<ITargetProvider> t, unique_ptr<IDronePhysics> d)
    : solver(std::move(s))
    , targets(std::move(t))
    , drone(std::move(d))
  {
  }

  vector<SimStep> simulation() override
  {
    steps[0] = SimStep{.pos = drone->getPos(), .direction = drone->getDir(), .state = DroneState::STOPPED};
    stepCount++;
    state = std::make_unique<StateStopped>();
    float simTimeStep = drone->getConfig().simTimeStep;
    float timeScale = drone->getConfig().timeScale;
    while (hasNext()) {
      std::this_thread::sleep_for(std::chrono::duration<float>(simTimeStep / timeScale));
      steps[stepCount] = step();
      // LOG_Step(steps[stepCount], stepCount);
      if (canHit(drone->getConfig(), steps[stepCount])) {
        break;
      }
      stepCount++;
    }
    steps.resize(stepCount + 1);
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
      const Target target = targets->getTarget(i);
      SolverResult res = solver->solve(drone->getConfig(), drone->getPos(), target, drone->getAmmo());
      results.at(i) = res;
      if (results.at(selectedTargetIdx).timeToPos > res.timeToPos) {
        selectedTargetIdx = i;
      }
    }
    newSimStep.predictedTarget = results.at(selectedTargetIdx).interpolatedTargetPos;
    newSimStep.dropPoint = results.at(selectedTargetIdx).balisticPoint;
    newSimStep.targetIdx = selectedTargetIdx;

    float theta = getTheta(newSimStep.pos, newSimStep.predictedTarget);
    drone->cmd(DroneCommand{.dir = theta});

    newSimStep.direction = drone->getDir();
    newSimStep.state = drone->getState();
    newSimStep.pos = drone->getPos();
    currentTime += drone->getConfig().simTimeStep;
    return newSimStep;
  }

  void reset() override { stepCount = 0; }

  void changeSolver(unique_ptr<IBallisticSolver> s) override { solver = std::move(s); }

  bool hasNext() override { return stepCount < MAX_STEPS; }  // MAX_STEPS

  int getStepsCount() override { return stepCount; }

  bool isThreadReady() override { return stepCount == 0; }
  bool start() override
  {
    if (isThreadReady()) {
      started = true;
      return true;
    }
    return started;
  }

  bool stop() override
  {
    started = false;
    return started;
  }

  void run() override
  {
    vector<SimStep> steps = simulation();
    Utils::saveSimulation(steps);
  }

private:
  int stepCount = 0;
  float currentTime = 0;
  vector<SimStep> steps = vector<SimStep>(MAX_STEPS);
  std::unique_ptr<IDroneState> state;
  bool started = false;

  float getTheta(const Coord& a, const Coord& b)
  {
    Coord delta = b - a;
    return atan2(delta.y, delta.x);
  }

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

  bool canHit(const DroneConfig& config, const SimStep& step)
  {
    float d = Coord::getDistance(step.pos, step.dropPoint);
    return d < config.hitRadius && step.state == DroneState::MOVING;
  }
};
