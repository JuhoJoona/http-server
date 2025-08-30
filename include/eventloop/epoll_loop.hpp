#ifndef EPOLL_LOOP_HPP
#define EPOLL_LOOP_HPP

// Only compile on Linux systems that support epoll
#ifdef __linux__

#include "event_loop.hpp"
#include <sys/epoll.h>

class EpollLoop : public EventLoop {
public:
  EpollLoop();
  ~EpollLoop();

  bool addSocket(int fd, int events) override;
  bool removeSocket(int fd) override;
  bool modifySocket(int fd, int events) override;
  int waitForEvents(void *events, int maxEvents, int timeout = -1) override;

private:
  int epfd;
};

#else
#error "EpollLoop is only supported on Linux systems"
#endif

#endif