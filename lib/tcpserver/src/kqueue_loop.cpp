#include "../include/tcpserver/kqueue_loop.hpp"
#include <sys/event.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

namespace tcpserver {

KqueueLoop::KqueueLoop() : kqueueFd_(-1), initialized_(false) {
    kqueueFd_ = kqueue();
    if (kqueueFd_ == -1) {
        std::cerr << "Failed to create kqueue instance: " << strerror(errno) << std::endl;
        return;
    }
    initialized_ = true;
}

KqueueLoop::~KqueueLoop() {
    if (kqueueFd_ != -1) {
        close(kqueueFd_);
    }
}

bool KqueueLoop::addSocket(int fd, int events) {
    if (!initialized_) {
        return false;
    }
    
    struct kevent changes[2];
    int changeCount = 0;
    
    if (events & READ_EVENT) {
        EV_SET(&changes[changeCount], fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
        changeCount++;
    }
    if (events & WRITE_EVENT) {
        EV_SET(&changes[changeCount], fd, EVFILT_WRITE, EV_ADD, 0, 0, nullptr);
        changeCount++;
    }
    
    if (kevent(kqueueFd_, changes, changeCount, nullptr, 0, nullptr) == -1) {
        std::cerr << "Failed to add socket to kqueue: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool KqueueLoop::removeSocket(int fd) {
    if (!initialized_) {
        return false;
    }
    
    struct kevent changes[2];
    int changeCount = 0;
    
    EV_SET(&changes[changeCount], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    changeCount++;
    EV_SET(&changes[changeCount], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
    changeCount++;
    
    if (kevent(kqueueFd_, changes, changeCount, nullptr, 0, nullptr) == -1) {
        std::cerr << "Failed to remove socket from kqueue: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool KqueueLoop::modifySocket(int fd, int events) {
    if (!initialized_) {
        return false;
    }
    
    // First remove the socket
    if (!removeSocket(fd)) {
        return false;
    }
    
    // Then add it back with new events
    return addSocket(fd, events);
}

int KqueueLoop::waitForEvents(void *events, int maxEvents, int timeout) {
    if (!initialized_) {
        return -1;
    }
    
    struct kevent *kevents = static_cast<struct kevent*>(events);
    struct timespec timeoutSpec;
    struct timespec *timeoutPtr = nullptr;
    
    if (timeout >= 0) {
        timeoutSpec.tv_sec = timeout / 1000;
        timeoutSpec.tv_nsec = (timeout % 1000) * 1000000;
        timeoutPtr = &timeoutSpec;
    }
    
    return kevent(kqueueFd_, nullptr, 0, kevents, maxEvents, timeoutPtr);
}

} // namespace tcpserver
