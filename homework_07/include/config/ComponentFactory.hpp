#pragma once

#include <memory>
#include <string>
#include "interfaces/ITargetProvider.hpp"
#include "interfaces/IBallisticSolver.hpp"
#include "interfaces/IConfigLoader.hpp"

enum class SolverType { ANALYTICAL, TABLE };
enum class ProviderType { JSON };
enum class LoaderType { FILE };

std::unique_ptr<IBallisticSolver> createSolver(SolverType type);
std::unique_ptr<ITargetProvider> createProvider(ProviderType type, const string param);
std::unique_ptr<IConfigLoader> createLoader(LoaderType type);