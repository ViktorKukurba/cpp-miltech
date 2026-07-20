#include <string>

#include "config/ComponentFactory.hpp"
#include "interfaces/IConfigLoader.hpp"
#include "interfaces/ITargetProvider.hpp"
#include "interfaces/IBallisticSolver.hpp"

#include "../config/FileConfigLoader.cpp"
#include "../solvers/AnalyticalSolver.cpp"
#include "../providers/JsonTargetProvider.cpp"

#include "iostream"

const map<LoaderType, function<IConfigLoader*()>> loaderFactories = {{LoaderType::FILE, []() { return new FileConfigLoader; }}};
const map<ProviderType, function<ITargetProvider*(const std::string&)>> providerFactories = {
  {ProviderType::JSON, [](const string param) { return new JsonTargetProvider(param); }}};
const map<SolverType, function<IBallisticSolver*()>> solverFactories = {{SolverType::ANALYTICAL, []() { return new AnalyticalSolver; }}};

IConfigLoader* createLoader(LoaderType type)
{
  return loaderFactories.at(type)();
};

ITargetProvider* createProvider(ProviderType type, const string param)
{
  cout << param;
  return providerFactories.at(type)(param);
};

IBallisticSolver* createSolver(SolverType type)
{
  return solverFactories.at(type)();
};
