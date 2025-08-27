#include "../../include/net/server.hpp"
#include "../../include/http/router.hpp"
#include <sys/event.h>
#include <memory>
#include "../../include/net/connection.hpp"
#include <iostream>

Server::Server(int port, Router& router) : port(port), router(router) {
    // create socket first
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        throw std::runtime_error("Failed to create socket");
    }

    //set socket options
    int opt = 1;
    setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //bind socket to ip and port
    sockaddr_in address;
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port); // Use the port parameter

    if (bind(this->sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(this->sockfd);
        throw std::runtime_error("Failed to bind socket");
    }

    //listen for incoming connections
    if (listen(this->sockfd, 10) == -1) {
        std::cerr << "Failed to listen for incoming connections" << std::endl;
        close(this->sockfd);
        throw std::runtime_error("Failed to listen for incoming connections");
    }

    // Now create kqueue and register the socket
    kq = kqueue();
    if (kq == -1) throw std::runtime_error("kqueue creation failed");

    // register server_fd for read (incoming connections)
    struct kevent ev_set;
    EV_SET(&ev_set, this->sockfd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kq, &ev_set, 1, nullptr, 0, nullptr) == -1) {
        throw std::runtime_error("Failed to register server socket with kqueue");
    }
}

Server::~Server() {
    // Close all active connections
    connections.clear();
    close(this->sockfd);
}

void Server::start() {
    eventLoop();
}

void Server::stop() {
    // Close all active connections
    connections.clear();
    close(this->sockfd);
}

void Server::eventLoop() {
    const int MAX_EVENTS = 64;
    struct kevent events[MAX_EVENTS];

    while (true) {
        int n = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
        if (n == -1) {
            std::cerr << "kevent error: " << strerror(errno) << std::endl;
            continue;
        }
        
        for (int i = 0; i < n; i++) {
            if (events[i].ident == static_cast<uintptr_t>(this->sockfd)) {
                int client_fd = accept(this->sockfd, nullptr, nullptr);
                if (client_fd != -1) {
                    auto connection = std::make_shared<Connection>(client_fd, kq, router);
                    connections[client_fd] = connection;
                }
            } else {
                // Handle client connection events
                auto it = connections.find(events[i].ident);
                if (it != connections.end()) {
                    it->second->handleEvent(events[i]);
                    
                    // If connection is closed, remove it from the map
                    if (it->second->isClosed()) {
                        connections.erase(it);
                    }
                }
            }
        }
    }
}



