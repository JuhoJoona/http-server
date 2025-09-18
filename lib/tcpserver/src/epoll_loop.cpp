#include "../include/tcpserver/epoll_loop.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

namespace tcpserver {

EpollLoop::EpollLoop() : epollFd_(-1), initialized_(false) {
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd_ == -1) {
        std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
        return;
    }
    initialized_ = true;
}

EpollLoop::~EpollLoop() {
    if (epollFd_ != -1) {
        close(epollFd_);
    }
}

bool EpollLoop::addSocket(int fd, int events) {
    if (!initialized_) {
        return false;
    }
    
    epoll_event event;
    event.data.fd = fd;
    event.events = 0;
    
    if (events & READ_EVENT) {
        event.events |= EPOLLIN;
    }
    if (events & WRITE_EVENT) {
        event.events |= EPOLLOUT;
    }
    if (events & ERROR_EVENT) {
        event.events |= EPOLLERR;
    }
    
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) == -1) {
        std::cerr << "Failed to add socket to epoll: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool EpollLoop::removeSocket(int fd) {
    if (!initialized_) {
        return false;
    }
    
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        std::cerr << "Failed to remove socket from epoll: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool EpollLoop::modifySocket(int fd, int events) {
    if (!initialized_) {
        return false;
    }
    
    epoll_event event;
    event.data.fd = fd;
    event.events = 0;
    
    if (events & READ_EVENT) {
        event.events |= EPOLLIN;
    }
    if (events & WRITE_EVENT) {
        event.events |= EPOLLOUT;
    }
    if (events & ERROR_EVENT) {
        event.events |= EPOLLERR;
    }
    
    if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) == -1) {
        std::cerr << "Failed to modify socket in epoll: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

int EpollLoop::waitForEvents(void *events, int maxEvents, int timeout) {
    if (!initialized_) {
        return -1;
    }
    
    epoll_event *epollEvents = static_cast<epoll_event*>(events);
    return epoll_wait(epollFd_, epollEvents, maxEvents, timeout);
}

} // namespace tcpserver
