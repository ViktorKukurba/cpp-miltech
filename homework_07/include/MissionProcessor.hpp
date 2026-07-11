#pragma once

#include <cstdint>
#include <vector>
#include "interfaces/IBallisticSolver.hpp"
#include "interfaces/IConfigLoader.hpp"

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

class IMissionProcessor {
public:
  virtual ~IMissionProcessor() = default;
  virtual void init(IConfigLoader* configSource) = 0;
  virtual vector<SimStep> simulation() = 0;
  virtual SimStep step() = 0;
  virtual void reset() = 0;
  virtual void changeSolver(IBallisticSolver* s) = 0;
  virtual bool hasNext() = 0;
  virtual int getStepsCount() = 0;
};