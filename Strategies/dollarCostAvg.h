#pragma once

#include "Strategy.h"
#include <cstddef>
#include <optional>
#include <string>

class dollarCostAvg : public Strategy
{
private:
    std::string symbol;
    std::size_t intervalDays;
    double contribution;
    std::size_t barsSeen{};

public:
    dollarCostAvg(std::string symbol, std::size_t intervalDays = 30,
                  double contribution = 100.0);

    std::optional<OrderEvent>
    onData(const DataEvent &dataEvent) override;
};
