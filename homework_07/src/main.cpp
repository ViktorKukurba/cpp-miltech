#include <memory>
#include "iostream"
#include "MissionProcessor.cpp"
#include "config/ComponentFactory.cpp"
#include "DronePhysics.cpp"
#include "interfaces/IConfigLoader.hpp"
#include "interfaces/ITargetProvider.hpp"

int main()
{
  try {
    unique_ptr<IBallisticSolver> analytical = createSolver(SolverType::TABLE);
    unique_ptr<ITargetProvider> provider = createProvider(ProviderType::JSON, "homework_07/data/targets.json");
    unique_ptr<IConfigLoader> configSource = createLoader(LoaderType::FILE);

    unique_ptr<IDronePhysics> dronePhysics = make_unique<DronePhysics>();
    dronePhysics->init(std::move(configSource));

    provider->setTimeStep(dronePhysics->getConfig().targetTimeStep);
    provider->setTimeScale(dronePhysics->getConfig().targetTimeStep);

    ITargetProvider* providerPtr = provider.get();
    IDronePhysics* dronePhysicsPtr = dronePhysics.get();

    MissionProcessor mission(std::move(analytical), std::move(provider), std::move(dronePhysics));

    std::thread providerThread(&ITargetProvider::run, providerPtr);

    std::thread physicsThread(&IDronePhysics::run, dronePhysicsPtr);
    std::thread missionThread(&MissionProcessor::run, &mission);

    while (!providerPtr->isThreadReady() && !dronePhysicsPtr->isThreadReady())
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

    providerPtr->start();
    dronePhysicsPtr->start();
    missionThread.join();
    dronePhysicsPtr->stop();
    providerPtr->stop();

    providerThread.join();
    physicsThread.join();
  }
  catch (...) {
    cerr << "Error" << endl;
    return 1;
  }
  return 0;
}