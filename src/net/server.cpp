#include "../../include/net/server.hpp"
#include "../../include/eventloop/event_loop_factory.hpp"
#include "../../include/http/router.hpp"
#include "../../include/net/connection.hpp"
#include "../../include/net/connection_event.hpp"
#include <iostream>
#include <memory>

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

  // bind socket to ip and port
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port); // Use the port parameter

  if (bind(this->sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
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
  eventLoop = EventLoopFactory::createEventLoop();
  if (!eventLoop) {
    throw std::runtime_error("Event loop creation failed");
  }

  // Register server socket with event loop
  if (!eventLoop->addSocket(this->sockfd, EventLoop::READ_EVENT)) {
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
  void *events[MAX_EVENTS]; // Platform-agnostic events

  while (true) {
    int n = eventLoop->waitForEvents(events, MAX_EVENTS);
    if (n == -1) {
      std::cerr << "Event loop error" << std::endl;
      continue;
    }

    for (int i = 0; i < n; i++) {
      // Convert platform-specific event to ConnectionEvent
      ConnectionEvent event = convertToConnectionEvent(events[i]);

      if (event.ident == static_cast<uintptr_t>(this->sockfd)) {
        int client_fd = accept(this->sockfd, nullptr, nullptr);
        if (client_fd != -1) {
          auto connection =
              std::make_shared<Connection>(client_fd, *eventLoop, router);
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
  // This will be implemented differently for each platform
  // For now, return a basic event structure
  ConnectionEvent event;
  // Platform-specific conversion logic will go here
  return event;
}
