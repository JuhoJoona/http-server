#include "../include/tcpserver/tcp_server.hpp"
#include "../include/tcpserver/event_loop_factory.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <errno.h>

// Platform-specific includes
#ifdef __linux__
#include <sys/epoll.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#include <sys/event.h>
#endif

namespace tcpserver {

TcpServer::TcpServer(int port, std::shared_ptr<ConnectionHandler> handler)
    : port_(port), sockfd_(-1), running_(false), handler_(handler) {
    
    // Create socket
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
        throw std::runtime_error("Failed to create socket");
    }
    
    // Set socket options
    int opt = 1;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Set socket to non-blocking mode
    int flags = fcntl(sockfd_, F_GETFL, 0);
    fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK);
    
    // Bind socket
    memset(&address_, 0, sizeof(address_));
    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port_);
    
    if (bind(sockfd_, (struct sockaddr*)&address_, sizeof(address_)) < 0) {
        std::string error = "Failed to bind socket on port " + std::to_string(port_) + ": " + strerror(errno);
        close(sockfd_);
        throw std::runtime_error(error);
    }
    
    // Listen for connections
    if (listen(sockfd_, 10) == -1) {
        std::string error = "Failed to listen for connections on port " + std::to_string(port_) + ": " + strerror(errno);
        close(sockfd_);
        throw std::runtime_error(error);
    }
    
    // Create event loop
    eventLoop_ = EventLoopFactory::createEventLoop();
    if (!eventLoop_) {
        close(sockfd_);
        throw std::runtime_error("Failed to create event loop");
    }
    
    // Register server socket with event loop
    if (!eventLoop_->addSocket(sockfd_, EventLoop::READ_EVENT)) {
        close(sockfd_);
        throw std::runtime_error("Failed to register server socket with event loop");
    }
}

TcpServer::~TcpServer() {
    stop();
}

void TcpServer::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    std::cout << "TCP Server starting on port " << port_ << std::endl;
    eventLoop();
}

void TcpServer::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Close all connections
    connections_.clear();
    
    // Close server socket
    if (sockfd_ != -1) {
        close(sockfd_);
        sockfd_ = -1;
    }
    
    std::cout << "TCP Server stopped" << std::endl;
}

void TcpServer::eventLoop() {
    const int MAX_EVENTS = 64;
    
    // Platform-specific event buffer
    #ifdef __linux__
    epoll_event events[MAX_EVENTS];
    #elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    struct kevent events[MAX_EVENTS];
    #endif
    
    while (running_) {
        int n = eventLoop_->waitForEvents(events, MAX_EVENTS);
        if (n == -1) {
            if (errno == EINTR) {
                continue; // Interrupted by signal, continue
            }
            std::cerr << "Event loop error: " << strerror(errno) << std::endl;
            break;
        }
        
        for (int i = 0; i < n; i++) {
            #ifdef __linux__
            int fd = events[i].data.fd;
            uint32_t eventFlags = events[i].events;
            #elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
            int fd = events[i].ident;
            uint32_t eventFlags = events[i].filter;
            #endif
            
            if (fd == sockfd_) {
                // New connection
                handleNewConnection();
            } else {
                // Client connection event
                handleClientEvent(fd, eventFlags);
            }
        }
    }
}

void TcpServer::handleNewConnection() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    int clientFd = accept(sockfd_, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Failed to accept connection: " << strerror(errno) << std::endl;
        }
        return;
    }
    
    // Create connection object
    auto connection = std::make_shared<Connection>(clientFd, *eventLoop_);
    connections_[clientFd] = connection;
    
    // Notify handler
    if (handler_) {
        handler_->onConnection(connection);
    }
}

void TcpServer::handleClientEvent(int clientFd, int events) {
    auto it = connections_.find(clientFd);
    if (it == connections_.end()) {
        return;
    }
    
    auto connection = it->second;
    
    #ifdef __linux__
    if (events & EPOLLIN) {
        connection->handleRead();
        // Notify handler about new data
        if (handler_ && !connection->isClosed()) {
            std::string data = connection->getReadBuffer();
            if (!data.empty()) {
                handler_->onData(connection, data);
                connection->clearReadBuffer();
            }
        }
    }
    if (events & EPOLLOUT) {
        connection->handleWrite();
    }
    #elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    if (events == EVFILT_READ) {
        connection->handleRead();
        // Notify handler about new data
        if (handler_ && !connection->isClosed()) {
            std::string data = connection->getReadBuffer();
            if (!data.empty()) {
                handler_->onData(connection, data);
                connection->clearReadBuffer();
            }
        }
    }
    if (events == EVFILT_WRITE) {
        connection->handleWrite();
    }
    #endif
    
    // Remove closed connections
    if (connection->isClosed()) {
        if (handler_) {
            handler_->onDisconnect(connection);
        }
        connections_.erase(it);
    }
}

} // namespace tcpserver
