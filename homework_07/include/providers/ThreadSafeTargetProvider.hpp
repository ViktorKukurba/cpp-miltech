#pragma once

#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include "interfaces/ITargetProvider.hpp"
#include "interfaces/IRunnable.hpp"
#include "types.hpp"

// ThreadSafeTargetProvider — loads targets from a JSON file and simulates a
// moving clock in a background thread. Thread-safe access via mutex.
class ThreadSafeTargetProvider : public ITargetProvider, public IRunnable {
public:
  explicit ThreadSafeTargetProvider(const std::string& filePath);
  ~ThreadSafeTargetProvider() override;

  // IRunnable
  void start() override;
  void stop() override;

  // ITargetProvider
  int getTargetCount() override;
  int getTimeSteps() override;
  Target getTarget(int index) override;

private:
  std::string filePath_;
  std::vector<std::vector<Coord>> targets_;
  int targetCount_{0};
  int timeSteps_{0};
  int currentStep_{0};

  std::thread thread_;
  std::atomic<bool> running_{false};
  mutable std::mutex mutex_;

  void loadTargets();
  void run();
};
