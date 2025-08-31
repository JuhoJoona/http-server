#include "../../include/eventloop/event_loop_factory.hpp"
#include "../../include/eventloop/epoll_loop.hpp"
#include <stdexcept>

std::unique_ptr<EventLoop> EventLoopFactory::createEventLoop() {
#ifdef __linux__
    return std::make_unique<EpollLoop>();
#else
    throw std::runtime_error("No supported event loop implementation for this platform");
#endif
}

std::unique_ptr<EventLoop> EventLoopFactory::createKqueueLoop() {
    throw std::runtime_error("Kqueue not implemented");
}

std::unique_ptr<EventLoop> EventLoopFactory::createEpollLoop() {
#ifdef __linux__
    return std::make_unique<EpollLoop>();
#else
    throw std::runtime_error("Epoll not supported on this platform");
#endif
}
