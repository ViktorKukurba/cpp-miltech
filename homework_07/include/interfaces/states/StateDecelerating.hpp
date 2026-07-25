#pragma once

#include <cmath>
#include <memory>

#include "interfaces/IDroneState.hpp"

#include "types.hpp"

class StateDecelerating : public IDroneState {
public:
  std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;

  DroneState type() const override { return DroneState::DECELERATING; };

  const char* name() const override { return "Decelerating"; }

  virtual ~StateDecelerating() = default;
};
