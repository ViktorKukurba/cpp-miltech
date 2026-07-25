
#include <cmath>
#include <memory>

#include "interfaces/IDroneState.hpp"
#include "interfaces/states/StateAccelerating.hpp"
#include "interfaces/states/StateMoving.hpp"

std::unique_ptr<IDroneState> StateAccelerating::execute(DroneContext& ctx)
{
  const float a = ctx.cfg.attackSpeed * ctx.cfg.attackSpeed / (2 * ctx.cfg.accelPath);
  ctx.speed = ctx.speed + a * ctx.cfg.simTimeStep;
  if (ctx.speed >= ctx.cfg.attackSpeed) {
    ctx.speed = ctx.cfg.attackSpeed;
    return make_unique<StateMoving>();
  }
  return nullptr;
}
