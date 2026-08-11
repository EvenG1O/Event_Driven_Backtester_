#pragma once

#include "../FillEvent/FilleEvent.h"
#include "../OrderEvent/OrderEvent.h"
#include "../types/Candle.h"
#include <memory>

class SimulatedBroker {
public:
  virtual ~SimulatedBroker() = default;

  virtual std::unique_ptr<FillEvent>
  fillOrder(const OrderEvent &orderEvent, const Candle &fillCandle) const;
};
