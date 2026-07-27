#include <cmath>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include "DronePhysics.hpp"
#include "interfaces/states/StateStopped.hpp"
#include "MissionProcessor.hpp"
#include "utils.hpp"

MissionProcessor::MissionProcessor(std::unique_ptr<IBallisticSolver> s, std::unique_ptr<ITargetProvider> t, std::unique_ptr<DronePhysics> d)
  : solver(std::move(s))
  , targets(std::move(t))
  , drone(std::move(d))
{
}

vector<SimStep> MissionProcessor::simulation()
{
  int currentStep = 0;
  steps[currentStep] = SimStep{.pos = drone->getPos(), .direction = drone->getDir(), .state = DroneState::STOPPED};
  stepCount = 1;
  state = std::make_unique<StateStopped>();

  float simTimeStep = drone->getConfig().simTimeStep;
  float timeScale = drone->getConfig().timeScale;

  while (hasNext()) {
    std::this_thread::sleep_for(std::chrono::duration<float>(simTimeStep / timeScale));

    currentStep = stepCount;
    steps[currentStep] = step();
    if (canHit(drone->getConfig(), steps[currentStep])) {
      break;
    }

    stepCount = currentStep + 1;
  }

  steps.resize(stepCount);
  return steps;
}

SimStep MissionProcessor::step()
{
  const int targetsCount = targets->getTargetCount();
  SimStep simStep = steps[stepCount - 1];
  SimStep newSimStep = SimStep{.pos = simStep.pos, .direction = simStep.direction, .state = simStep.state, .targetIdx = simStep.targetIdx};
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

void MissionProcessor::reset()
{
  stepCount = 0;
}

void MissionProcessor::changeSolver(unique_ptr<IBallisticSolver> s)
{
  solver = std::move(s);
}

bool MissionProcessor::hasNext()
{
  return stepCount < MAX_STEPS;
}

int MissionProcessor::getStepsCount()
{
  return stepCount;
}

bool MissionProcessor::isThreadReady()
{
  return stepCount == 0;
}

bool MissionProcessor::start()
{
  if (isThreadReady()) {
    started = true;
    return true;
  }
  return started;
}

bool MissionProcessor::stop()
{
  started = false;
  return started;
}

void MissionProcessor::run()
{
  vector<SimStep> steps = simulation();
  Utils::saveSimulation(steps);
}

float MissionProcessor::getTheta(const Coord& a, const Coord& b)
{
  Coord delta = b - a;
  return atan2(delta.y, delta.x);
}

void MissionProcessor::LOG_Coord(const Coord& c)
{
  LOG("Coords: x=" + to_string(c.x) + "; y=" + to_string(c.y));
}

void MissionProcessor::LOG_Step(const SimStep& step, int idx)
{
  LOG("Simulation step: " + to_string(idx));
  LOG_Coord(step.pos);
  LOG("Direction: " + to_string(step.direction));
  LOG("State: " + to_string(step.state));
  LOG("Target: " + to_string(step.targetIdx));
}

bool MissionProcessor::canHit(const DroneConfig& config, const SimStep& step)
{
  float d = Coord::getDistance(step.pos, step.dropPoint);
  return d < config.hitRadius && step.state == DroneState::MOVING;
}
