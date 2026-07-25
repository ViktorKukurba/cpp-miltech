#include <thread>
#include "fstream"
#include "json.hpp"

#include "interfaces/ITargetProvider.hpp"

#include "iostream"
#include "types.hpp"

using json = nlohmann::json;

class ThreadSafeTargetProvider : public ITargetProvider {
public:
  ThreadSafeTargetProvider(const string file) { loadTargets(file); }

  void setTimeStep(float timeStep) override { targetTimeStep = timeStep; }
  void setTimeScale(float tS) override { timeScale = tS; }
  int getTargetCount() override { return targetCount; };

  int getTimeSteps() override { return timeSteps; };

  Target getTarget(int index) override
  {
    const Coord current = data.at(index).at(currentStep);
    if (currentStep == 0) {
      return Target{.pos = current, .velocity = Coord{0, 0}};
    }
    const Coord prev = data.at(index).at(currentStep - 1);

    return Target{.pos = current, .velocity = current - prev};
  };

  bool isThreadReady() override { return timeSteps != 0 && targetTimeStep != 0; }
  bool start() override
  {
    if (isThreadReady()) {
      started = true;
      return true;
    }

    return started;
  }

  bool stop() override
  {
    started = false;
    return started;
  }

  void run() override { simulation(); }

private:
  vector<vector<Coord>> data;
  float targetTimeStep = 0;
  float timeScale = 0;
  int targetCount = 0;
  int timeSteps = 0;
  int currentStep = 0;
  vector<Target> prevTargets;
  vector<Target> targets;
  bool started = false;
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

    data.assign(targetCount, vector<Coord>(timeSteps));
    for (int i = 0; i < targetCount; i++) {
      for (int j = 0; j < timeSteps; j++) {
        data[i][j].x = jt["targets"][i]["positions"][j]["x"];
        data[i][j].y = jt["targets"][i]["positions"][j]["y"];
      }
    }
  }

  void simulation()
  {
    while (started) {
      std::this_thread::sleep_for(std::chrono::duration<float>(targetTimeStep / timeScale));

      ++currentStep;
      if (currentStep == timeSteps - 1)
        currentStep = 0;
    }
  }
};
