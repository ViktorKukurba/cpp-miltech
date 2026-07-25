#pragma once

#include <cmath>
#include <memory>

#include "interfaces/IDroneState.hpp"
#include "interfaces/states/StateDecelerating.hpp"
#include "interfaces/states/StateStopped.hpp"
#include "types.hpp"

std::unique_ptr<IDroneState> StateDecelerating::execute(DroneContext& ctx)
{
  const float a = -ctx.cfg.attackSpeed * ctx.cfg.attackSpeed / (2 * ctx.cfg.accelPath);
  ctx.speed = ctx.speed + a * ctx.cfg.simTimeStep;
  if (ctx.speed <= 0) {
    ctx.speed = 0;
    return std::make_unique<StateStopped>();
  }
  return nullptr;
}
