#include <memory>
#include "MissionProcessor.cpp"
#include "config/ComponentFactory.cpp"
#include "interfaces/IConfigLoader.hpp"
#include "interfaces/ITargetProvider.hpp"
#include "utils.hpp"

int main()
{
  try {
    unique_ptr<IBallisticSolver> analytical = createSolver(SolverType::TABLE);
    unique_ptr<ITargetProvider> provider = createProvider(ProviderType::JSON, "homework_07/data/targets.json");
    unique_ptr<IConfigLoader> configSource = createLoader(LoaderType::FILE);

    MissionProcessor mission(std::move(analytical), std::move(provider));
    mission.init(std::move(configSource));
    vector<SimStep> steps = mission.simulation();
    Utils::saveSimulation(steps);
  }
  catch (...) {
    cerr << "Error" << endl;
    return 1;
  }
  return 0;
}