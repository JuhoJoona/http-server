#include "../../include/net/connection.hpp"
#include "../../include/http/router.hpp"
#include <iostream>
#include <sys/event.h>
#include <cstring>
#include <errno.h> 

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
    std::string received_data;
    int total_bytes = 0;
    
    while (true) {
        int bytes = recv(sockfd, buffer, sizeof(buffer), 0);
        
        if (bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            close();
            return;
        }
        
        if (bytes == 0) {
            close();
            return;
        }
        
        received_data.append(buffer, bytes);
        total_bytes += bytes;
        
        if (isCompleteHttpRequest(received_data)) {
            break;
        }
        
        if (total_bytes > 1024 * 1024) {
            std::cerr << "Request too large, closing connection" << std::endl;
            close();
            return;
        }
    }
    
    Request request;
    if (!HttpParser::parseRequest(received_data, request)) {
        std::string response = "HTTP/1.1 400 Bad Request\r\n"
                               "Content-Length: 0\r\n\r\n";
        send(sockfd, response.c_str(), response.size(), 0);
        close();
        return;
    }
    
    Response response = router.route(request);
    std::string responseStr = response.to_string();
    
    send(sockfd, responseStr.c_str(), responseStr.size(), 0);
    
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

bool Connection::isCompleteHttpRequest(const std::string& data) {
    size_t header_end = data.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        return false;
    }
    
    size_t content_length_pos = data.find("Content-Length:");
    if (content_length_pos != std::string::npos) {
        size_t line_end = data.find("\r\n", content_length_pos);
        if (line_end != std::string::npos) {
            std::string length_str = data.substr(content_length_pos + 15, 
                                               line_end - content_length_pos - 15);
            try {
                int content_length = std::stoi(length_str);
                size_t body_start = header_end + 4;
                if (data.length() < body_start + content_length) {
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing content length: " << e.what() << std::endl;
                return false;
            }
        }
    }
    
    return true;
}
