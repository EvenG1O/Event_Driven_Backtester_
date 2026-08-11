#pragma once

#include "Strategy.h"
#include "Indicators/MovingAverage.h"
#include <cstddef>
#include <optional>
#include <string>

class MovingAverageCrossoverStrategy : public Strategy {
private:
  std::string symbol;
  std::size_t fastWindow;
  std::size_t slowWindow;
  double quoteAmount;
  std::size_t barsSeen = 0;
  MovingAverage fastAverage;
  MovingAverage slowAverage;
  bool hasPreviousAverages = false;
  double previousFastAverage = 0.0;
  double previousSlowAverage = 0.0;

  static int toIndicatorWindow(std::size_t window);

public:
  MovingAverageCrossoverStrategy(std::string symbol, std::size_t fastWindow,
                                 std::size_t slowWindow,
                                 double quoteAmount);

  std::optional<OrderEvent>
  onData(const DataEvent &dataEvent) override;
};
