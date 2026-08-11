#pragma once
#include "Strategy.h"
#include <optional>
#include <string>

class buyAndHold : public Strategy
{
private:
    std::string symbol;
    double quoteAmount;
    bool hasBought = false;

public:
    buyAndHold(std::string symbol, double quoteAmount);

    std::optional<OrderEvent>
    onData(const DataEvent &dataEvent) override;
};
