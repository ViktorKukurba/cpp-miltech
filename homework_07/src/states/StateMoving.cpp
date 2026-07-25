#pragma once

#include <cmath>
#include <memory>

#include "interfaces/states/StateMoving.hpp"
#include "interfaces/states/StateDecelerating.hpp"
#include "utils.hpp"

std::unique_ptr<IDroneState> StateMoving::execute(DroneContext& ctx)
{
  float delta = Utils::normalizeAngle(ctx.desiredDir - ctx.direction);

  if (std::fabs(delta) > ctx.cfg.turnThreshold) {
    return std::make_unique<StateDecelerating>();
  }
  return nullptr;
}
