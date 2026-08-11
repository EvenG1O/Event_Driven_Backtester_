#pragma once
#include "../Event/Event.h"
#include "../types/Candle.h"
#include "../types/OrderSide.h"
#include <string>

class FillEvent : public Event
{
private:
  std::string symbol;
  OrderSide side;
  double fillPrice;
  double fillQuantity;
  double commission;
  Candle fillCandle;

public:
  FillEvent(const std::string &symbol, const OrderSide side, double price,
            double qty, double commission, const Candle &fillCandle)
      : Event(EventType::Fill), symbol(symbol), side(side),
        fillPrice(price), fillQuantity(qty), commission(commission),
        fillCandle(fillCandle) {}

  const std::string &getSymbol() const { return symbol; }
  OrderSide getSide() const { return side; }
  double getFillPrice() const { return fillPrice; }
  double getFillQuantity() const { return fillQuantity; }
  double getCommission() const { return commission; }
  const Candle &getFillCandle() const { return fillCandle; }
};
