#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "interfaces/ITargetProvider.hpp"
#include "types.hpp"

// ThreadSafeTargetProvider — loads targets from a JSON file and simulates a
// moving clock in a background thread. Thread-safe access via mutex.
// It inherits from ITargetProvider because that interface already exposes the
// runnable API used by main.cpp.
class ThreadSafeTargetProvider : public ITargetProvider {
public:
  explicit ThreadSafeTargetProvider(const std::string& filePath);
  ~ThreadSafeTargetProvider() override;

  // IRunnable
  bool start() override;
  bool stop() override;
  bool isThreadReady() override;
  void run() override;

  // ITargetProvider
  int getTargetCount() override;
  int getTimeSteps() override;
  Target getTarget(int index) override;
  void setTimeStep(float timeStep) override;
  void setTimeScale(float timeScale) override;

private:
  std::string filePath_;
  std::vector<std::vector<Coord>> targets_;
  int targetCount_{0};
  int timeSteps_{0};
  int currentStep_{0};

  float targetTimeStep_{0.0f};
  float timeScale_{0.0f};

  std::thread thread_;
  std::atomic<bool> running_{false};
  mutable std::mutex mutex_;

  void loadTargets();
  void simulation();
};
