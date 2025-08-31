#include "../../include/eventloop/event_loop_factory.hpp"
#include <stdexcept>
#include <memory>

// Platform-specific includes
#ifdef __linux__
#include "../../include/eventloop/epoll_loop.hpp"
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#include "../../include/eventloop/kqueue_loop.hpp"
#endif

std::unique_ptr<EventLoop> EventLoopFactory::createEventLoop() {
#ifdef __linux__
    return std::make_unique<EpollLoop>();
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
      defined(__OpenBSD__) || defined(__DragonFly__)
    return std::make_unique<KqueueLoop>();
#else
    throw std::runtime_error("No supported event loop implementation for this platform");
#endif
}

std::unique_ptr<EventLoop> EventLoopFactory::createKqueueLoop() {
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(__DragonFly__)
    return std::make_unique<KqueueLoop>();
#else
    throw std::runtime_error("Kqueue not supported on this platform");
#endif
}

std::unique_ptr<EventLoop> EventLoopFactory::createEpollLoop() {
#ifdef __linux__
    return std::make_unique<EpollLoop>();
#else
    throw std::runtime_error("Epoll not supported on this platform");
#endif
}
