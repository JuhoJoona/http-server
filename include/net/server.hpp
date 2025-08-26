#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <memory>
#include "connection.hpp"

// Forward declaration
class Router;

class Server {
    public:
        Server(int port, Router& router);
        ~Server();
        void start();
        void stop();

        void eventLoop();

        int sockfd;
        sockaddr_in address;
        int port;
        int kq;
        Router& router;
        
        std::map<int, std::shared_ptr<Connection> > connections;

};

#endif