#ifndef TCP_EPOLL_LOOP_HPP
#define TCP_EPOLL_LOOP_HPP

#include "event_loop.hpp"
#include <sys/epoll.h>

namespace tcpserver {

/**
 * Linux epoll-based event loop implementation
 */
class EpollLoop : public EventLoop {
public:
    EpollLoop();
    ~EpollLoop() override;

    bool addSocket(int fd, int events) override;
    bool removeSocket(int fd) override;
    bool modifySocket(int fd, int events) override;
    int waitForEvents(void *events, int maxEvents, int timeout = -1) override;

private:
    int epollFd_;
    bool initialized_;
};

} // namespace tcpserver

#endif // TCP_EPOLL_LOOP_HPP
