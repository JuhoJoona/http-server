#include "../include/tcpserver/connection.hpp"
#include "../include/tcpserver/event_loop.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

namespace tcpserver {

Connection::Connection(int sockfd, EventLoop& eventLoop) 
    : sockfd_(sockfd), eventLoop_(eventLoop), closed_(false), clientPort_(0) {
    
    // Get client address information
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    if (getpeername(sockfd_, (struct sockaddr*)&clientAddr, &clientAddrLen) == 0) {
        clientAddress_ = inet_ntoa(clientAddr.sin_addr);
        clientPort_ = ntohs(clientAddr.sin_port);
    } else {
        clientAddress_ = "unknown";
        clientPort_ = 0;
    }
    
    // Set socket to non-blocking mode
    int flags = fcntl(sockfd_, F_GETFL, 0);
    fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK);
    
    // Register with event loop for read events
    registerWithEventLoop();
}

Connection::~Connection() {
    close();
}

bool Connection::send(const std::string& data) {
    return send(data.c_str(), data.length());
}

bool Connection::send(const char* data, size_t length) {
    if (closed_) {
        return false;
    }
    
    // Add to write buffer
    writeBuffer_.append(data, length);
    
    // Try to send immediately
    ssize_t bytesSent = ::send(sockfd_, writeBuffer_.c_str(), writeBuffer_.length(), MSG_NOSIGNAL);
    
    if (bytesSent > 0) {
        writeBuffer_.erase(0, bytesSent);
        
        // If there's still data to send, register for write events
        if (!writeBuffer_.empty()) {
            eventLoop_.modifySocket(sockfd_, EventLoop::READ_EVENT | EventLoop::WRITE_EVENT);
        }
        
        return true;
    } else if (bytesSent == 0) {
        // Connection closed by peer
        close();
        return false;
    } else {
        // Error occurred
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Would block, register for write events
            eventLoop_.modifySocket(sockfd_, EventLoop::READ_EVENT | EventLoop::WRITE_EVENT);
            return true;
        } else {
            close();
            return false;
        }
    }
}

void Connection::close() {
    if (!closed_) {
        closed_ = true;
        unregisterFromEventLoop();
        ::close(sockfd_);
    }
}

bool Connection::isClosed() const {
    return closed_;
}

std::string Connection::getClientAddress() const {
    return clientAddress_;
}

int Connection::getClientPort() const {
    return clientPort_;
}

std::string Connection::getReadBuffer() const {
    return readBuffer_;
}

void Connection::clearReadBuffer() {
    readBuffer_.clear();
}

void Connection::handleRead() {
    char buffer[4096];
    ssize_t bytesRead = ::recv(sockfd_, buffer, sizeof(buffer), 0);
    
    if (bytesRead > 0) {
        readBuffer_.append(buffer, bytesRead);
        // Note: The actual data handling will be done by the TcpServer
        // which will call the ConnectionHandler
    } else if (bytesRead == 0) {
        // Connection closed by peer
        close();
    } else {
        // Error occurred
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            close();
        }
    }
}

void Connection::handleWrite() {
    if (writeBuffer_.empty()) {
        return;
    }
    
    ssize_t bytesSent = ::send(sockfd_, writeBuffer_.c_str(), writeBuffer_.length(), MSG_NOSIGNAL);
    
    if (bytesSent > 0) {
        writeBuffer_.erase(0, bytesSent);
        
        // If all data sent, unregister from write events
        if (writeBuffer_.empty()) {
            eventLoop_.modifySocket(sockfd_, EventLoop::READ_EVENT);
        }
    } else if (bytesSent == 0) {
        close();
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            close();
        }
    }
}

void Connection::registerWithEventLoop() {
    eventLoop_.addSocket(sockfd_, EventLoop::READ_EVENT);
}

void Connection::unregisterFromEventLoop() {
    eventLoop_.removeSocket(sockfd_);
}

} // namespace tcpserver
