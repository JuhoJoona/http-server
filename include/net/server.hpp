#ifndef SERVER_HPP
#define SERVER_HPP

#include "../eventloop/event_loop.hpp"
#include "connection.hpp"
#include "connection_event.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// Forward declaration
class Router;

class Server {
public:
  Server(int port, Router &router);
  ~Server();
  void start();
  void stop();
  void eventLoop();

private:
  int sockfd;
  sockaddr_in address;
  int port;
  std::unique_ptr<EventLoop> eventLoop; // Platform-specific implementation
  Router &router;
  std::map<int, std::shared_ptr<Connection>> connections;

  // Helper method to convert platform-specific events
  ConnectionEvent convertToConnectionEvent(void *platformEvent);
};

#endif