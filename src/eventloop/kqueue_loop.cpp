#include "../../include/eventloop/kqueue_loop.hpp"
#include <sys/event.h>
#include <unistd.h>
#include <stdexcept>

KqueueLoop::KqueueLoop() {
    kq = kqueue();
    if (kq == -1) {
        throw std::runtime_error("Failed to create kqueue instance");
    }
}

bool KqueueLoop::addSocket(int fd, int events) {
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
    return kevent(kq, &kev, 1, nullptr, 0, nullptr) == 0;
}

bool KqueueLoop::removeSocket(int fd) {
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    return kevent(kq, &kev, 1, nullptr, 0, nullptr) == 0;
}

bool KqueueLoop::modifySocket(int fd, int events) {
    // For kqueue, we need to remove and re-add the socket with new events
    if (!removeSocket(fd)) {
        return false;
    }
    return addSocket(fd, events);
}

int KqueueLoop::waitForEvents(void *events, int maxEvents, int timeout) {
    struct timespec ts;
    struct timespec *timeout_ptr = nullptr;
    
    if (timeout >= 0) {
        ts.tv_sec = timeout / 1000;
        ts.tv_nsec = (timeout % 1000) * 1000000;
        timeout_ptr = &ts;
    }
    
    return kevent(kq, nullptr, 0, static_cast<struct kevent*>(events), maxEvents, timeout_ptr);
}

KqueueLoop::~KqueueLoop() {
    if (kq != -1) {
        close(kq);
    }
}
