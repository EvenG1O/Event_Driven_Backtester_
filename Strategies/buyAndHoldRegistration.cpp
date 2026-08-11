#include "buyAndHold.h"
#include "../strategyBase/strategyRegistration.h"
#include <memory>

namespace
{
[[maybe_unused]] const StrategyRegistration registration(
    "buy_and_hold",
    []
    { return std::make_unique<buyAndHold>("BTCUSDT", 100.0); });
}
