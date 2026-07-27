#include <chrono>
#include <memory>
#include <thread>
#include "DronePhysics.hpp"
#include "interfaces/IConfigLoader.hpp"
#include "interfaces/IDroneState.hpp"
#include "interfaces/states/StateStopped.hpp"
#include "types.hpp"

void DronePhysics::init(std::unique_ptr<IConfigLoader> configSource)
{
  std::lock_guard<std::mutex> lock(mutex_);
  configSource->load();
  ammo = configSource->getAmmoParams();
  config = configSource->getConfig();
  pos = config.startPos;
  prevPos = config.startPos;
  dir = config.initialDir;
  state = std::make_unique<StateStopped>();
  isInitialized = true;
}

void DronePhysics::cmd(DroneCommand data)
{
  std::lock_guard<std::mutex> lock(mutex_);
  desiredDir = data.dir;
};

Coord DronePhysics::getPos()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return pos;
}

float DronePhysics::getDir()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return dir;
}

DroneState DronePhysics::getState()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return state->type();
}

DroneConfig DronePhysics::getConfig()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return config;
}

AmmoParams DronePhysics::getAmmo()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return ammo;
}

void DronePhysics::run()
{
  while (true) {
    float timeStep = 0.0f;

    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (!started) {
        break;
      }

      timeStep = config.physicsTimeStep / config.timeScale;
      DroneContext ctx({.desiredDir = desiredDir, .direction = dir, .speed = speed, .cfg = config});
      auto next = state->execute(ctx);
      speed = ctx.speed;
      dir = ctx.direction;
      movePos();

      if (next) {
        state = std::move(next);
      }
    }

    if (timeStep > 0.0f) {
      std::this_thread::sleep_for(std::chrono::duration<float>(timeStep));
    }
  }
}

bool DronePhysics::isThreadReady()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return isInitialized;
}

bool DronePhysics::start()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (isInitialized) {
    started = true;
    return true;
  }

  return false;
}

bool DronePhysics::stop()
{
  std::lock_guard<std::mutex> lock(mutex_);
  started = false;
  return true;
}

float DronePhysics::getCurrentSpeed()
{
  std::lock_guard<std::mutex> lock(mutex_);
  const float speed = Coord::getDistance(pos, prevPos) / config.physicsTimeStep;
  return speed;
}

void DronePhysics::movePos()
{
  prevPos = pos;
  pos = pos.move(dir, config.physicsTimeStep, speed);
}
