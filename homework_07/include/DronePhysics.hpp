#pragma once

#include <memory>
#include <mutex>
#include "interfaces/IConfigLoader.hpp"
#include "interfaces/IRunnable.hpp"
#include "interfaces/IDroneState.hpp"

class DronePhysics : public IRunnable {
public:
  virtual void init(std::unique_ptr<IConfigLoader> configSource);
  virtual void cmd(DroneCommand data);
  virtual Coord getPos();
  virtual float getDir();
  virtual DroneState getState();
  virtual DroneConfig getConfig();
  virtual AmmoParams getAmmo();
  void run() override;
  bool isThreadReady() override;
  bool start() override;
  bool stop() override;

private:
  AmmoParams ammo;
  DroneConfig config;
  Coord prevPos;
  Coord pos;
  float dir{0.0f};
  float desiredDir{0.0f};
  float speed{0.0f};
  std::unique_ptr<IDroneState> state;
  bool started{false};
  bool isInitialized{false};
  std::mutex mutex_;

  float getCurrentSpeed();
  void movePos();
};
