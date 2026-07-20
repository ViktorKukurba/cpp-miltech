#include "fstream"
#include "json.hpp"

#include "providers/JsonTargetProvider.hpp"

#include "iostream"

using json = nlohmann::json;

class JsonTargetProvider : public IJsonTargetProvider {
public:
  JsonTargetProvider(const string file) { loadTargets(file); }
  int getTargetCount() override { return targetCount; };
  vector<Coord> getTarget(int index) override { return targets.at(index); };
  Coord getTargetCoord(int index, float) override { return targets.at(index).at(0); };
  int getTimeSteps() override { return timeSteps; };

private:
  vector<vector<Coord>> targets;
  int targetCount;
  int timeSteps;
  void loadTargets(const string file)
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
};
