#include "../../include/eventloop/epoll_loop.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>

EpollLoop::EpollLoop() {
    epfd = epoll_create1(0);
    if (epfd == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }
}

bool EpollLoop::addSocket(int fd, int events) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN; // Always listen for read events
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == 0;
}

bool EpollLoop::removeSocket(int fd) {
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr) == 0;
}

bool EpollLoop::modifySocket(int fd, int events) {
    epoll_event event;
    event.data.fd = fd;
    event.events = events;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == 0;
}

int EpollLoop::waitForEvents(void *events, int maxEvents, int timeout) {
    return epoll_wait(epfd, static_cast<epoll_event*>(events), maxEvents, timeout);
}

EpollLoop::~EpollLoop() {
    if (epfd != -1) {
        close(epfd);
    }
}
