#ifndef TCP_EVENT_LOOP_HPP
#define TCP_EVENT_LOOP_HPP

#include <functional>
#include <sys/socket.h>

namespace tcpserver {

/**
 * Abstract event loop interface for cross-platform event-driven I/O
 */
class EventLoop {
public:
    virtual ~EventLoop() = default;

    // Core event loop operations
    virtual bool addSocket(int fd, int events) = 0;
    virtual bool removeSocket(int fd) = 0;
    virtual bool modifySocket(int fd, int events) = 0;
    virtual int waitForEvents(void *events, int maxEvents, int timeout = -1) = 0;

    // Event types
    static const int READ_EVENT = 0x01;
    static const int WRITE_EVENT = 0x02;
    static const int ERROR_EVENT = 0x04;
};

} // namespace tcpserver

#endif // TCP_EVENT_LOOP_HPP
