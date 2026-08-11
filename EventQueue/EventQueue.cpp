#include "EventQueue.h"


void EventQueue::pushEvent(std::unique_ptr<Event> e) {
  events.push(std::move(e));
}


std::unique_ptr<Event> EventQueue::pop() {
  if (events.empty()) {
    return nullptr;
  }

  auto e = std::move(events.front());
  events.pop();
  return e;
}

bool EventQueue::empty() const { return events.empty(); }
