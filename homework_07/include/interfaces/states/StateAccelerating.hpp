#pragma once

#include <cmath>
#include <memory>

#include "interfaces/IDroneState.hpp"

class StateAccelerating : public IDroneState {
public:
  std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;

  const char* name() const override { return "Accelerating"; }

  DroneState type() const override { return DroneState::ACCELERATING; };

  virtual ~StateAccelerating() = default;
};
