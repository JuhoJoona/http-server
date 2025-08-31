#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "../eventloop/event_loop.hpp"
#include "../http/parser.hpp"
#include "../http/request.hpp"
#include "connection_event.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// Forward declaration
class Router;

class Connection {
public:
  Connection(int sockfd, EventLoop &eventLoop, Router &router);
  ~Connection();

  void handleEvent(const ConnectionEvent &event);
  bool isClosed() const;
  int getSocket() const { return sockfd; }

private:
  void read();
  void close();
  void registerWithEventLoop();
  void unregisterFromEventLoop();
  bool isCompleteHttpRequest(const std::string &data);

  int sockfd;
  EventLoop &eventLoop;
  Router &router;
  bool closed;
};

#endif