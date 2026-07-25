#include "interfaces/IConfigLoader.hpp"
#include "interfaces/IDroneState.hpp"
#include "interfaces/states/StateStopped.hpp"
#include "types.hpp"
#include "interfaces/IDronePhysics.hpp"
#include <thread>

class DronePhysics : public IDronePhysics {
public:
  void init(unique_ptr<IConfigLoader> configSource) override
  {
    configSource->load();
    ammo = configSource->getAmmoParams();
    config = configSource->getConfig();
    pos = config.startPos;
    prevPos = config.startPos;
    dir = config.initialDir;
    state = std::make_unique<StateStopped>();
    isInitialized = true;
  }

  void cmd(DroneCommand data) override { desiredDir = data.dir; };

  Coord getPos() override { return pos; }

  float getDir() override { return dir; }

  DroneState getState() override { return state->type(); }

  DroneConfig getConfig() override { return config; }

  AmmoParams getAmmo() override { return ammo; }

  void run() override
  {
    while (started) {
      std::this_thread::sleep_for(std::chrono::duration<float>(config.physicsTimeStep / config.timeScale));
      DroneContext ctx({.desiredDir = desiredDir, .direction = dir, .speed = speed, .cfg = config});
      auto next = state->execute(ctx);
      speed = ctx.speed;
      dir = ctx.direction;
      movePos();

      if (next) {
        state = std::move(next);
      }
    }
  }

  bool isThreadReady() override { return isInitialized; }
  bool start() override
  {
    if (isThreadReady()) {
      started = true;
      return true;
    }

    return started;
  }

  bool stop() override
  {
    started = false;
    return started;
  }

private:
  AmmoParams ammo;
  DroneConfig config;
  Coord prevPos;
  Coord pos;
  float dir;
  float desiredDir;
  float speed = 0;
  std::unique_ptr<IDroneState> state;
  bool started = false;
  bool isInitialized = false;

  float getCurrentSpeed()
  {
    const float speed = Coord::getDistance(pos, prevPos) / config.physicsTimeStep;
    return speed;
  }

  void movePos()
  {
    prevPos = pos;
    pos = pos.move(dir, config.physicsTimeStep, speed);
  }
};
