#include "MovingAverageCrossoverStrategy.h"
#include "../strategyBase/strategyRegistration.h"
#include <memory>

namespace
{
[[maybe_unused]] const StrategyRegistration registration(
    "moving_average",
    []
    {
        return std::make_unique<MovingAverageCrossoverStrategy>(
            "BTCUSDT", 2, 3, 100.0);
    });
}
