#pragma once

#include <string>
#include "../types/EventType.h"

class Event
{
public:
  EventType type;

  explicit Event(EventType type) : type(type) {}
  virtual ~Event() = default;
};
