#include <fstream>
#include <stdexcept>
#include "json.hpp"
#include "providers/JsonTargetProvider.hpp"

using json = nlohmann::json;
using namespace std;

JsonTargetProvider::JsonTargetProvider(const string file)
{
  loadTargets(file);
}

int JsonTargetProvider::getTargetCount()
{
  return targetCount;
}

Target JsonTargetProvider::getTarget(int index)
{
  return Target{.pos = targets.at(index).at(0)};
}

int JsonTargetProvider::getTimeSteps()
{
  return timeSteps;
}

void JsonTargetProvider::loadTargets(const string file)
{
  ifstream ft(file);
  if (!ft) {
    throw runtime_error("Could not open file: " + file);
  }
  json jt;
  ft >> jt;
  targetCount = jt["targetCount"];
  timeSteps = jt["timeSteps"];

  targets.assign(targetCount, vector<Coord>(timeSteps));
  for (int i = 0; i < targetCount; i++) {
    for (int j = 0; j < timeSteps; j++) {
      targets[i][j].x = jt["targets"][i]["positions"][j]["x"];
      targets[i][j].y = jt["targets"][i]["positions"][j]["y"];
    }
  }
}
