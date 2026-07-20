#pragma once

#include <string>
#include "interfaces/ITargetProvider.hpp"
#include "interfaces/IBallisticSolver.hpp"
#include "interfaces/IConfigLoader.hpp"

enum class SolverType { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType { FILE };

IBallisticSolver* createSolver(SolverType type);
ITargetProvider* createProvider(ProviderType type, const string param);
IConfigLoader* createLoader(LoaderType type);