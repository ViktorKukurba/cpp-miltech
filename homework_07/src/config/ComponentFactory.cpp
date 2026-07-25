#include <string>

#include "config/ComponentFactory.hpp"
#include "interfaces/IConfigLoader.hpp"
#include "interfaces/ITargetProvider.hpp"
#include "interfaces/IBallisticSolver.hpp"

#include "../config/FileConfigLoader.cpp"
#include "../solvers/AnalyticalSolver.cpp"
// #include "../providers/JsonTargetProvider.cpp"
#include "../providers/ThreadSafeTargetProvider.cpp"

#include "iostream"
#include "../solvers/TableSolver.cpp"

const map<LoaderType, function<std::unique_ptr<IConfigLoader>()>> loaderFactories = {
  {LoaderType::FILE, []() { return std::make_unique<FileConfigLoader>(); }}};

const map<ProviderType, function<std::unique_ptr<ITargetProvider>(const std::string)>> providerFactories = {
  {ProviderType::JSON, [](const std::string param) { return std::make_unique<ThreadSafeTargetProvider>(param); }}};

const map<SolverType, function<std::unique_ptr<IBallisticSolver>()>> solverFactories = {
  {SolverType::ANALYTICAL, []() { return std::make_unique<AnalyticalSolver>(); }},
  {SolverType::TABLE, []() { return std::make_unique<TableSolver>(); }}};

std::unique_ptr<IConfigLoader> createLoader(LoaderType type)
{
  return loaderFactories.at(type)();
};

std::unique_ptr<ITargetProvider> createProvider(ProviderType type, const string param)
{
  return providerFactories.at(type)(param);
};

std::unique_ptr<IBallisticSolver> createSolver(SolverType type)
{
  return solverFactories.at(type)();
};
