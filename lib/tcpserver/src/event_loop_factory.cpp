#include "../include/tcpserver/event_loop_factory.hpp"
#include "../include/tcpserver/epoll_loop.hpp"
#include "../include/tcpserver/kqueue_loop.hpp"

namespace tcpserver {

std::unique_ptr<EventLoop> EventLoopFactory::createEventLoop() {
    #ifdef __linux__
    return std::make_unique<EpollLoop>();
    #elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    return std::make_unique<KqueueLoop>();
    #else
    #error "Unsupported platform"
    #endif
}

} // namespace tcpserver
