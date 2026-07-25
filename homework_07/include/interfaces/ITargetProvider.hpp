#pragma once

#include "interfaces/IRunnable.hpp"

#include "types.hpp"

class ITargetProvider : public IRunnable {
public:
  virtual int getTargetCount() = 0;
  virtual int getTimeSteps() = 0;
  virtual Target getTarget(int index) = 0;
  virtual ~ITargetProvider() = default;
  virtual void setTimeStep(float timeStep) = 0;
  virtual void setTimeScale(float timeScale) = 0;
};