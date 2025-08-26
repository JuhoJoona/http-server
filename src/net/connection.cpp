#include "../../include/net/connection.hpp"
#include "../../include/http/router.hpp"
#include <iostream>
#include <sys/event.h>
#include <cstring>

Connection::Connection(int sockfd, int kq, Router& router) : sockfd(sockfd), kq(kq), router(router), closed(false) {
    registerWithKqueue();
}

Connection::~Connection() {
    if (!closed) {
        close();
    }
}

void Connection::handleEvent(const struct kevent& event) {
    if (event.filter == EVFILT_READ) {
        read();
    }
}

bool Connection::isClosed() const {
    return closed;
}

void Connection::read() {
    char buffer[4096];
    int bytes = recv(sockfd, buffer, sizeof(buffer), 0);
    
    if (bytes <= 0) {
        close();
        return;
    }
    
    // Parse the HTTP request
    std::string rawData(buffer, bytes);
    Request request;
    
    if (!HttpParser::parseRequest(rawData, request)) {
        // Send 400 Bad Request if parsing fails
        std::string response = "HTTP/1.1 400 Bad Request\r\n"
                               "Content-Length: 0\r\n\r\n";
        send(sockfd, response.c_str(), response.size(), 0);
        close();
        return;
    }
    
    // Log the parsed request
    std::cout << "Received " << request.method << " request for " << request.path << std::endl;
    
    // Use the stored router to handle the HTTP request
    Response response = router.route(request);
    std::string responseStr = response.to_string();
    
    send(sockfd, responseStr.c_str(), responseStr.size(), 0);
    
    // Close the connection after sending response (HTTP/1.0 style)
    close();
}

void Connection::close() {
    if (!closed) {
        unregisterFromKqueue();
        ::close(sockfd);
        closed = true;
    }
}

void Connection::registerWithKqueue() {
    struct kevent ev_set;
    EV_SET(&ev_set, sockfd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kq, &ev_set, 1, nullptr, 0, nullptr) == -1) {
        std::cerr << "Failed to register client with kqueue" << std::endl;
        close();
    }
}

void Connection::unregisterFromKqueue() {
    struct kevent ev_set;
    EV_SET(&ev_set, sockfd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    kevent(kq, &ev_set, 1, nullptr, 0, nullptr);
}
