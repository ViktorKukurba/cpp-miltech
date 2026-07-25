#pragma once

#include <memory>
#include "types.hpp"

class IDroneState {
public:
  virtual ~IDroneState() = default;

  // Виконати логіку стану, повернути наступний стан.
  // Якщо стан не змінився — повернути nullptr
  // (головний цикл залишить поточний).
  virtual std::unique_ptr<IDroneState> execute(DroneContext& ctx) = 0;

  virtual const char* name() const = 0;

  virtual DroneState type() const = 0;
};
