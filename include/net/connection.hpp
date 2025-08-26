#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/event.h>
#include <string>
#include "../http/request.hpp"
#include "../http/parser.hpp"

// Forward declaration
class Router;

class Connection {
    public:
        Connection(int sockfd, int kq, Router& router);
        ~Connection();

        void handleEvent(const struct kevent& event);
        bool isClosed() const;
        int getSocket() const { return sockfd; }

    private:
        void read();
        void close();
        void registerWithKqueue();
        void unregisterFromKqueue();

        int sockfd;
        int kq;
        Router& router;
        bool closed;
};

#endif