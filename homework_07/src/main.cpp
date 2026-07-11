#include "mission_processor.cpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

void saveSimulation(SimStep* steps, int n)
{
  json out;
  out["totalSteps"] = n;
  out["steps"] = json::array();
  for (int i = 0; i < n; i++) {
    SimStep s = steps[i];
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
  std::ofstream fout(SIMULATION_FILE);
  fout << out.dump(2);
}

int main()
{
  try {
    AnalyticalSolver analytical;
    JsonTargetProvider provider("homework_07/data/targets.json");
    FileConfigLoader configSource;

    MissionProcessor mission(&analytical, &provider);
    mission.init(&configSource);
    SimStep* steps = mission.simulation();
    saveSimulation(steps, mission.getStepsCount());
  }
  catch (...) {
    cerr << "Error" << endl;
    return 1;
  }

  return 0;
}