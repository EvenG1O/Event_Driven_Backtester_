#pragma once

#include "../DataEvent/DataEvent.h"
#include "../OrderEvent/OrderEvent.h"
#include <optional>

class Strategy {
public:
  virtual ~Strategy() = default;

  virtual std::optional<OrderEvent> onData(const DataEvent &dataEvent) = 0;
};
