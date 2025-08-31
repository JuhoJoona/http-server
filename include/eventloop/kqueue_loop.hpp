#ifndef KQUEUE_LOOP_HPP
#define KQUEUE_LOOP_HPP

// Only compile on macOS/BSD systems that support kqueue
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(__DragonFly__)

#include "event_loop.hpp"
#include <sys/event.h>

class KqueueLoop : public EventLoop {
public:
  KqueueLoop();
  ~KqueueLoop();

  bool addSocket(int fd, int events) override;
  bool removeSocket(int fd) override;
  bool modifySocket(int fd, int events) override;
  int waitForEvents(void *events, int maxEvents, int timeout = -1) override;

private:
  int kq;
};

#else
#error "KqueueLoop is only supported on macOS and BSD systems"
#endif

#endif