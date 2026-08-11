#pragma once

#include "../Event/Event.h"
#include "../types/Candle.h"

class DataEvent : public Event
{
private:
  Candle candle;

public:
  explicit DataEvent(const Candle &bar) : Event(EventType::Data), candle(bar) {}

  const Candle &getCandle() const { return candle; }
};
