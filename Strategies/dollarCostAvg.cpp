#include "dollarCostAvg.h"
#include "../helpers/QuoteSizing.h"
#include "../types/OrderTypes.h"
#include <stdexcept>
#include <utility>

dollarCostAvg::dollarCostAvg(
    std::string symbol, std::size_t intervalDays,
    double contribution)
    : symbol(std::move(symbol)), intervalDays(intervalDays),
      contribution(contribution)
{
    if (intervalDays == 0 || contribution <= 0.0)
    {
        throw std::invalid_argument(
            "Dollar-cost-average settings must be positive");
    }
}

std::optional<OrderEvent>
dollarCostAvg::onData(const DataEvent &dataEvent)
{
    ++barsSeen;

    if ((barsSeen - 1) % intervalDays != 0 ||
        dataEvent.getCandle().close <= 0.0)
    {
        return std::nullopt;
    }

    const double quantity = StrategySizing::quoteAmountToQuantity(
        contribution, dataEvent.getCandle().close);

    return OrderEvent(OrderType::Market, OrderSide::Buy, symbol,
                      dataEvent.getCandle().date_open,
                      dataEvent.getCandle().close,
                      quantity);
}
