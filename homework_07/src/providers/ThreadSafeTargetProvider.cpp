#include <chrono>
#include <fstream>
#include <stdexcept>
#include <thread>
#include "json.hpp"
#include "providers/ThreadSafeTargetProvider.hpp"

using json = nlohmann::json;

ThreadSafeTargetProvider::ThreadSafeTargetProvider(const std::string& filePath)
  : filePath_(filePath)
{
  loadTargets();
}

ThreadSafeTargetProvider::~ThreadSafeTargetProvider()
{
  stop();
}

void ThreadSafeTargetProvider::setTimeStep(float timeStep)
{
  std::lock_guard<std::mutex> lock(mutex_);
  targetTimeStep_ = timeStep;
}
void ThreadSafeTargetProvider::setTimeScale(float timeScale)
{
  std::lock_guard<std::mutex> lock(mutex_);
  timeScale_ = timeScale;
}
int ThreadSafeTargetProvider::getTargetCount()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return targetCount_;
}
int ThreadSafeTargetProvider::getTimeSteps()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return timeSteps_;
}

Target ThreadSafeTargetProvider::getTarget(int index)
{
  std::lock_guard<std::mutex> lock(mutex_);
  const Coord current = targets_.at(index).at(currentStep_);
  if (currentStep_ == 0) {
    return Target{.pos = current, .velocity = Coord{0, 0}};
  }
  const Coord prev = targets_.at(index).at(currentStep_ - 1);
  return Target{.pos = current, .velocity = current - prev};
}

bool ThreadSafeTargetProvider::isThreadReady()
{
  return timeSteps_ != 0 && targetTimeStep_ != 0.0f;
}

bool ThreadSafeTargetProvider::start()
{
  if (isThreadReady()) {
    running_ = true;
    thread_ = std::thread(&ThreadSafeTargetProvider::simulation, this);
    return true;
  }
  return false;
}

bool ThreadSafeTargetProvider::stop()
{
  running_ = false;
  if (thread_.joinable()) {
    thread_.join();
  }
  return true;
}

void ThreadSafeTargetProvider::run()
{
  simulation();
}

void ThreadSafeTargetProvider::loadTargets()
{
  std::ifstream ft(filePath_);
  if (!ft) {
    throw std::runtime_error("Could not open file: " + filePath_);
  }
  json jt;
  ft >> jt;
  targetCount_ = jt["targetCount"];
  timeSteps_ = jt["timeSteps"];

  targets_.assign(targetCount_, std::vector<Coord>(timeSteps_));
  for (int i = 0; i < targetCount_; i++) {
    for (int j = 0; j < timeSteps_; j++) {
      targets_[i][j].x = jt["targets"][i]["positions"][j]["x"];
      targets_[i][j].y = jt["targets"][i]["positions"][j]["y"];
    }
  }
}

void ThreadSafeTargetProvider::simulation()
{
  while (running_) {
    std::this_thread::sleep_for(std::chrono::duration<float>(targetTimeStep_ / timeScale_));
    {
      std::lock_guard<std::mutex> lock(mutex_);
      ++currentStep_;
      if (currentStep_ == timeSteps_ - 1) {
        currentStep_ = 0;
      }
    }
  }
}
