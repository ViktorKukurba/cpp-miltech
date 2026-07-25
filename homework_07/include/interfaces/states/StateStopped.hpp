#pragma once

#include <cmath>

#include "interfaces/IDroneState.hpp"

class StateStopped : public IDroneState {
public:
  std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
  const char* name() const override { return "Stopped"; }

  DroneState type() const override { return DroneState::STOPPED; };

  virtual ~StateStopped() = default;
};
