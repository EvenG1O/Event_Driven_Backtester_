#include "Broker.h"

std::unique_ptr<FillEvent>
SimulatedBroker::fillOrder(const OrderEvent &orderEvent,
                           const Candle &fillCandle) const {
  const double fillPrice =
      orderEvent.getOrder() == OrderType::Market ? fillCandle.close
                                                  : orderEvent.getPrice();

  return std::make_unique<FillEvent>(
      orderEvent.getSymbol(), orderEvent.getSide(), fillPrice,
      orderEvent.getQuantity(), 0.0, fillCandle);
}
