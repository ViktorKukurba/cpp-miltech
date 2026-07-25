#pragma once

#include <memory>
#include "interfaces/IConfigLoader.hpp"
#include "interfaces/IRunnable.hpp"
#include "types.hpp"

class IDronePhysics : public IRunnable {
public:
  virtual ~IDronePhysics() = default;
  virtual void init(unique_ptr<IConfigLoader> configSource) = 0;
  virtual void cmd(DroneCommand data) = 0;
  virtual Coord getPos() = 0;
  virtual float getDir() = 0;
  // virtual float getSpeed() = 0;
  virtual DroneState getState() = 0;
  // virtual void setTarget(const Coord& target) = 0;
  virtual DroneConfig getConfig() = 0;
  virtual AmmoParams getAmmo() = 0;
};
