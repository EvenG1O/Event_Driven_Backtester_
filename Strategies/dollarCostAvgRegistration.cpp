#include "dollarCostAvg.h"
#include "../strategyBase/strategyRegistration.h"
#include <memory>

namespace
{
[[maybe_unused]] const StrategyRegistration registration(
    "dollar_cost_avg",
    []
    { return std::make_unique<dollarCostAvg>("BTCUSDT", 30, 100.0); });
}
