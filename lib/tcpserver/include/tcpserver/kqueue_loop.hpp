#ifndef TCP_KQUEUE_LOOP_HPP
#define TCP_KQUEUE_LOOP_HPP

#include "event_loop.hpp"

// Forward declaration for kqueue structures
struct kevent;

namespace tcpserver {

/**
 * macOS/BSD kqueue-based event loop implementation
 */
class KqueueLoop : public EventLoop {
public:
    KqueueLoop();
    ~KqueueLoop() override;

    bool addSocket(int fd, int events) override;
    bool removeSocket(int fd) override;
    bool modifySocket(int fd, int events) override;
    int waitForEvents(void *events, int maxEvents, int timeout = -1) override;

private:
    int kqueueFd_;
    bool initialized_;
};

} // namespace tcpserver

#endif // TCP_KQUEUE_LOOP_HPP
