#include "json.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <iostream>
#include "fstream"

using json = nlohmann::json;
using namespace std;

namespace Utils {

void saveSimulation(vector<SimStep> steps)
{
  json out;
  out["totalSteps"] = steps.size();
  out["steps"] = json::array();
  for (SimStep s : steps) {
    json step;
    step["position"] = {{"x", s.pos.x}, {"y", s.pos.y}};
    step["direction"] = s.direction;
    step["state"] = s.state;
    step["targetIndex"] = s.targetIdx;
    step["dropPoint"] = {{"x", s.dropPoint.x}, {"y", s.dropPoint.y}};
    step["aimPoint"] = {{"x", s.aimPoint.x}, {"y", s.aimPoint.y}};
    step["predictedTarget"] = {{"x", s.predictedTarget.x}, {"y", s.predictedTarget.y}};
    out["steps"].push_back(step);
  }
  std::ofstream fout("homework_07/data/simulation.json");
  std::cout << "SAVED";
  fout << out.dump(2);
}

float normalizeAngle(float theta)
{
  return abs(atan2(sin(theta), cos(theta)));
}
}  // namespace Utils