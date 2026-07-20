#pragma once

#include <cmath>
#include <memory>

#include "interfaces/IDroneState.hpp"

class StateMoving : public IDroneState {
public:
  std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
  const char* name() const override { return "Moving"; }

  DroneState type() const override { return DroneState::MOVING; };

  virtual ~StateMoving() = default;
};
