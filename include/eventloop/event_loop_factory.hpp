#ifndef EVENT_LOOP_FACTORY_HPP
#define EVENT_LOOP_FACTORY_HPP

#include "event_loop.hpp"
#include <memory>

class EventLoopFactory {
public:
  static std::unique_ptr<EventLoop> createEventLoop();

private:
  static std::unique_ptr<EventLoop> createKqueueLoop();
  static std::unique_ptr<EventLoop> createEpollLoop();
};

#endif