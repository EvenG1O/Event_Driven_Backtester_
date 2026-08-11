#pragma once
#include "../Event/Event.h"
#include <memory>
#include <queue>

class EventQueue {

private:
  std::queue<std::unique_ptr<Event>> events;

public:
  void pushEvent(std::unique_ptr<Event> e);

  std::unique_ptr<Event> pop();

  bool empty() const;
};
