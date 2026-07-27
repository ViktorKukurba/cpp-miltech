#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include "interfaces/IBallisticSolver.hpp"
#include "interfaces/IDroneState.hpp"
#include "interfaces/IRunnable.hpp"
#include "interfaces/ITargetProvider.hpp"

#define ENABLE_LOG 1
#define ENABLE_DEBUG 0

#if ENABLE_LOG
#define LOG(msg) std::cout << "[LOG] " << msg << std::endl
#else
#define LOG(msg)
#endif

#if ENABLE_DEBUG
#define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define DEBUG(msg)
#endif

using namespace std;

const uint16_t MAX_STEPS = 10000;

class DronePhysics;

class IMissionProcessor : public IRunnable {
public:
  virtual ~IMissionProcessor() = default;
  virtual vector<SimStep> simulation() = 0;
  virtual SimStep step() = 0;
  virtual void reset() = 0;
  virtual void changeSolver(unique_ptr<IBallisticSolver> s) = 0;
  virtual bool hasNext() = 0;
  virtual int getStepsCount() = 0;
};

class MissionProcessor : public IMissionProcessor {
public:
  MissionProcessor(std::unique_ptr<IBallisticSolver> s, std::unique_ptr<ITargetProvider> t, std::unique_ptr<DronePhysics> d);

  vector<SimStep> simulation() override;
  SimStep step() override;
  void reset() override;
  void changeSolver(unique_ptr<IBallisticSolver> s) override;
  bool hasNext() override;
  int getStepsCount() override;
  bool isThreadReady() override;
  bool start() override;
  bool stop() override;
  void run() override;

private:
  int stepCount = 0;
  float currentTime = 0;
  vector<SimStep> steps = vector<SimStep>(MAX_STEPS);
  std::unique_ptr<IBallisticSolver> solver;
  std::unique_ptr<ITargetProvider> targets;
  std::unique_ptr<DronePhysics> drone;
  std::unique_ptr<IDroneState> state;
  bool started = false;

  float getTheta(const Coord& a, const Coord& b);
  void LOG_Coord(const Coord& c);
  void LOG_Step(const SimStep& step, int idx);
  bool canHit(const DroneConfig& config, const SimStep& step);
};