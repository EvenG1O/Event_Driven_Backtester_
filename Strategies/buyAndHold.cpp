#include "buyAndHold.h"
#include "../helpers/QuoteSizing.h"
#include "../types/OrderTypes.h"
#include <stdexcept>
#include <utility>

buyAndHold::buyAndHold(std::string symbol, double quoteAmount)
    : symbol(std::move(symbol)), quoteAmount(quoteAmount)
{
    if (quoteAmount <= 0.0)
    {
        throw std::invalid_argument(
            "Buy-and-hold quote amount must be positive");
    }
}

std::optional<OrderEvent>
buyAndHold::onData(const DataEvent &dataEvent)
{
    if (hasBought)
    {
        return std::nullopt;
    }

    hasBought = true;
    const Candle &candle = dataEvent.getCandle();
    const double quantity = StrategySizing::quoteAmountToQuantity(
        quoteAmount, candle.close);

    return OrderEvent(OrderType::Market, OrderSide::Buy, symbol,
                      candle.date_open, candle.close,
                      quantity);
}
