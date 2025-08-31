#include "../../include/net/server.hpp"
#include "../../include/eventloop/event_loop_factory.hpp"
#include "../../include/http/router.hpp"
#include "../../include/net/connection.hpp"
#include "../../include/net/connection_event.hpp"
#include <fcntl.h>
#include <iostream>
#include <memory>

// Platform-specific includes
#ifdef __linux__
#include <sys/epoll.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#include <sys/event.h>
#endif

Server::Server(int port, Router &router) : port(port), router(router) {
  // create socket first
  this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sockfd == -1) {
    std::cerr << "Failed to create socket" << std::endl;
    throw std::runtime_error("Failed to create socket");
  }

  // set socket options
  int opt = 1;
  setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  
  // Set socket to non-blocking mode for epoll
  int flags = fcntl(this->sockfd, F_GETFL, 0);
  fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);

  // bind socket to ip and port
  this->address.sin_family = AF_INET;
  this->address.sin_addr.s_addr = INADDR_ANY;
  this->address.sin_port = htons(port); // Use the port parameter

  if (bind(this->sockfd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0) {
    perror("bind failed");
    close(this->sockfd);
    throw std::runtime_error("Failed to bind socket");
  }

  // listen for incoming connections
  if (listen(this->sockfd, 10) == -1) {
    std::cerr << "Failed to listen for incoming connections" << std::endl;
    close(this->sockfd);
    throw std::runtime_error("Failed to listen for incoming connections");
  }

  // Create platform-specific event loop
      eventLoopImpl = EventLoopFactory::createEventLoop();
    if (!eventLoopImpl) {
    throw std::runtime_error("Event loop creation failed");
  }

  // Register server socket with event loop
      if (!eventLoopImpl->addSocket(this->sockfd, EventLoop::READ_EVENT)) {
    throw std::runtime_error(
        "Failed to register server socket with event loop");
  }
}

Server::~Server() {
  // Close all active connections
  connections.clear();
  close(this->sockfd);
}

void Server::start() { eventLoop(); }

void Server::stop() {
  // Close all active connections
  connections.clear();
  close(this->sockfd);
}

void Server::eventLoop() {
  const int MAX_EVENTS = 64;
  
  // Platform-specific event buffer
  #ifdef __linux__
  epoll_event events[MAX_EVENTS];
  #elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
  struct kevent events[MAX_EVENTS];
  #endif

  while (true) {
    int n = eventLoopImpl->waitForEvents(events, MAX_EVENTS);
    if (n == -1) {
      std::cerr << "Event loop error" << std::endl;
      continue;
    }

    for (int i = 0; i < n; i++) {
      // Convert platform-specific event to ConnectionEvent
      ConnectionEvent event = convertToConnectionEvent(&events[i]);

      if (event.ident == static_cast<uintptr_t>(this->sockfd)) {
        int client_fd = accept(this->sockfd, nullptr, nullptr);
        if (client_fd != -1) {
          auto connection =
              std::make_shared<Connection>(client_fd, *eventLoopImpl, router);
          connections[client_fd] = connection;
        }
      } else {
        // Handle client connection events
        auto it = connections.find(event.ident);
        if (it != connections.end()) {
          it->second->handleEvent(event);

          // If connection is closed, remove it from the map
          if (it->second->isClosed()) {
            connections.erase(it);
          }
        }
      }
    }
  }
}

// Helper method to convert platform-specific events to ConnectionEvent
ConnectionEvent Server::convertToConnectionEvent(void *platformEvent) {
  if (!platformEvent) {
    std::cerr << "Error: platformEvent is null" << std::endl;
    ConnectionEvent event;
    event.ident = 0;
    event.filter = ConnectionEvent::READ;
    return event;
  }
  
  ConnectionEvent event;
  
  #ifdef __linux__
  epoll_event *epollEvent = static_cast<epoll_event*>(platformEvent);
  if (!epollEvent) {
    std::cerr << "Error: Failed to cast platformEvent to epoll_event" << std::endl;
    event.ident = 0;
    event.filter = ConnectionEvent::READ;
    return event;
  }
  
  event.ident = epollEvent->data.fd;
  
  if (epollEvent->events & EPOLLIN) {
    event.filter = ConnectionEvent::READ;
  } else if (epollEvent->events & EPOLLOUT) {
    event.filter = ConnectionEvent::WRITE;
  } else {
    event.filter = ConnectionEvent::READ; // Default to read
  }
  
  #elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
  struct kevent *kev = static_cast<struct kevent*>(platformEvent);
  if (!kev) {
    std::cerr << "Error: Failed to cast platformEvent to kevent" << std::endl;
    event.ident = 0;
    event.filter = ConnectionEvent::READ;
    return event;
  }
  
  event.ident = kev->ident;
  
  if (kev->filter == EVFILT_READ) {
    event.filter = ConnectionEvent::READ;
  } else if (kev->filter == EVFILT_WRITE) {
    event.filter = ConnectionEvent::WRITE;
  } else {
    event.filter = ConnectionEvent::READ; // Default to read
  }
  
  #else
  #error "Unsupported platform"
  #endif
  
  return event;
}
