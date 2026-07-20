#pragma once

#include <cmath>
#include <memory>

#include "interfaces/IDroneState.hpp"

class StateTurning : public IDroneState {
public:
  std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;

  const char* name() const override { return "Turning"; }

  DroneState type() const override { return DroneState::TURNING; };

  virtual ~StateTurning() = default;
};
