#pragma once

#include <cmath>
#include <memory>

#include "interfaces/IDroneState.hpp"
#include "interfaces/states/StateStopped.hpp"
#include "interfaces/states/StateTurning.hpp"
#include "interfaces/states/StateAccelerating.hpp"
#include "utils.hpp"

std::unique_ptr<IDroneState> StateStopped::execute(DroneContext& ctx)
{
  float delta = Utils::normalizeAngle(ctx.desiredDir - ctx.direction);

  if (std::fabs(delta) > ctx.cfg.turnThreshold) {
    ctx.turnRemaining = std::fabs(delta) / ctx.cfg.angularSpeed;
    ctx.targetDir = ctx.desiredDir;
    return std::make_unique<StateTurning>();
  }
  ctx.direction = ctx.desiredDir;
  return std::make_unique<StateAccelerating>();
}