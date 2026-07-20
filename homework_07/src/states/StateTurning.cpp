#pragma once

#include <cmath>
#include <memory>

#include "interfaces/states/StateTurning.hpp"
#include "interfaces/states/StateAccelerating.hpp"
#include "utils.hpp"

float calculateTurn(float currentTheta, float targetTheta, float angularSpeed, float dt)
{
  float delta = targetTheta - currentTheta;

  // Normalize
  while (delta > M_PI)
    delta -= 2 * M_PI;
  while (delta < -M_PI)
    delta += 2 * M_PI;

  float turnStep = angularSpeed * dt;
  if (abs(delta) > turnStep) {
    delta = (delta > 0 ? 1 : -1) * turnStep;
  }
  currentTheta += delta;
  return currentTheta;
}

std::unique_ptr<IDroneState> StateTurning::execute(DroneContext& ctx)
{
  float delta = Utils::normalizeAngle(ctx.desiredDir - ctx.direction);
  if (delta > ctx.cfg.turnThreshold) {
    ctx.direction = Utils::normalizeAngle(calculateTurn(ctx.direction, ctx.desiredDir, ctx.cfg.angularSpeed, ctx.cfg.simTimeStep));
    return nullptr;
  }
  else {
    return std::make_unique<StateAccelerating>();
  }
}
