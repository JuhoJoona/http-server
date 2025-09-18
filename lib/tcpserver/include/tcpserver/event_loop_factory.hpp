#ifndef TCP_EVENT_LOOP_FACTORY_HPP
#define TCP_EVENT_LOOP_FACTORY_HPP

#include "event_loop.hpp"
#include <memory>

namespace tcpserver {

/**
 * Factory for creating platform-specific event loop implementations
 */
class EventLoopFactory {
public:
    /**
     * Create a platform-specific event loop
     * @return Unique pointer to the event loop implementation
     */
    static std::unique_ptr<EventLoop> createEventLoop();
};

} // namespace tcpserver

#endif // TCP_EVENT_LOOP_FACTORY_HPP
