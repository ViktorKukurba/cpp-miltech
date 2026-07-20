#pragma once

#include <vector>
#include "types.hpp"

class ITargetProvider {
public:
  virtual int getTargetCount() = 0;
  virtual int getTimeSteps() = 0;
  virtual vector<Coord> getTarget(int index) = 0;
  virtual Coord getTargetCoord(int index, float) = 0;
  virtual ~ITargetProvider() = default;
};