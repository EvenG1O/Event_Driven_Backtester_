#pragma once

#include "../Event/Event.h"
#include "../types/OrderSide.h"
#include "../types/OrderTypes.h"
#include <string>

class OrderEvent : public Event
{

private:
  OrderType order;
  OrderSide side;
  std::string symbol;
  std::string timestamp;
  double quantity;
  double price;

public:
  OrderEvent(OrderType orderT, OrderSide side, std::string symbol,
             std::string timestamp, double price, double quantity)
      : Event(EventType::Order), order(orderT), side(side), symbol(symbol),
        timestamp(timestamp), quantity(quantity), price(price) {}

  OrderType getOrder() const { return order; }
  OrderSide getSide() const { return side; }
  const std::string &getSymbol() const { return symbol; }
  const std::string &getTimestamp() const { return timestamp; }
  double getQuantity() const { return quantity; }
  double getPrice() const { return price; }
};
