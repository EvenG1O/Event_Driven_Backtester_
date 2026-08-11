#include "MovingAverageCrossoverStrategy.h"
#include "../helpers/QuoteSizing.h"
#include "../types/OrderTypes.h"
#include <limits>
#include <stdexcept>
#include <utility>

int MovingAverageCrossoverStrategy::toIndicatorWindow(
    std::size_t window)
{
    if (window == 0)
    {
        throw std::invalid_argument(
            "Moving average windows must be greater than zero");
    }

    if (window > static_cast<std::size_t>(
                     std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument(
            "Moving average window is too large");
    }

    return static_cast<int>(window);
}

MovingAverageCrossoverStrategy::MovingAverageCrossoverStrategy(
    std::string symbol, std::size_t fastWindow,
    std::size_t slowWindow, double quoteAmount)
    : symbol(std::move(symbol)), fastWindow(fastWindow),
      slowWindow(slowWindow), quoteAmount(quoteAmount),
      fastAverage(toIndicatorWindow(fastWindow)),
      slowAverage(toIndicatorWindow(slowWindow))
{
    if (fastWindow > slowWindow)
    {
        throw std::invalid_argument(
            "Fast moving average window must be less than or equal to slow window");
    }

    if (quoteAmount <= 0.0)
    {
        throw std::invalid_argument(
            "Moving-average quote amount must be positive");
    }
}

std::optional<OrderEvent>
MovingAverageCrossoverStrategy::onData(
    const DataEvent &dataEvent)
{
    ++barsSeen;

    const double fast = fastAverage.push(
        static_cast<float>(dataEvent.getCandle().close));
    const double slow = slowAverage.push(
        static_cast<float>(dataEvent.getCandle().close));

    if (barsSeen < slowWindow)
    {
        return std::nullopt;
    }

    if (!hasPreviousAverages)
    {
        previousFastAverage = fast;
        previousSlowAverage = slow;
        hasPreviousAverages = true;
        return std::nullopt;
    }

    const bool crossedAbove =
        previousFastAverage <= previousSlowAverage && fast > slow;
    const bool crossedBelow =
        previousFastAverage >= previousSlowAverage && fast < slow;

    previousFastAverage = fast;
    previousSlowAverage = slow;

    if (crossedAbove)
    {
        const double quantity = StrategySizing::quoteAmountToQuantity(
            quoteAmount, dataEvent.getCandle().close);

        return OrderEvent(OrderType::Market, OrderSide::Buy, symbol,
                          dataEvent.getCandle().date_open,
                          dataEvent.getCandle().close, quantity);
    }

    if (crossedBelow)
    {
        const double quantity = StrategySizing::quoteAmountToQuantity(
            quoteAmount, dataEvent.getCandle().close);

        return OrderEvent(OrderType::Market, OrderSide::Sell, symbol,
                          dataEvent.getCandle().date_open,
                          dataEvent.getCandle().close, quantity);
    }

    return std::nullopt;
}
