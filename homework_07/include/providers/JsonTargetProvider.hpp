#pragma once

#include <string>
#include <vector>
#include "interfaces/ITargetProvider.hpp"
#include "types.hpp"

class IJsonTargetProvider : public ITargetProvider {};

class JsonTargetProvider : public IJsonTargetProvider {
public:
  explicit JsonTargetProvider(const std::string file);
  int getTargetCount() override;
  Target getTarget(int index) override;
  int getTimeSteps() override;

private:
  std::vector<std::vector<Coord>> targets;
  int targetCount{0};
  int timeSteps{0};

  void loadTargets(const std::string file);
};