#include "MissionProcessor.cpp"
#include "config/ComponentFactory.cpp"
#include "utils.cpp"

int main()
{
  try {
    auto analytical = createSolver(SolverType::ANALYTICAL);
    auto provider = createProvider(ProviderType::JSON, "homework_07/data/targets.json");
    auto configSource = createLoader(LoaderType::FILE);

    MissionProcessor mission(analytical, provider);
    mission.init(configSource);
    vector<SimStep> steps = mission.simulation();
    saveSimulation(steps);
  }
  catch (...) {
    cerr << "Error" << endl;
    return 1;
  }

  return 0;
}