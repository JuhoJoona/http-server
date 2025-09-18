#include "http_connection_handler.hpp"
#include "../../lib/tcpserver/include/tcpserver/connection.hpp"
#include "../include/http/parser.hpp"
#include <iostream>
#include <sstream>

HttpConnectionHandler::HttpConnectionHandler(Router& router) : router_(router) {
}

void HttpConnectionHandler::onConnection(std::shared_ptr<tcpserver::Connection> connection) {
    // Initialize buffer for this connection
    connectionBuffers_[connection->getSocket()] = "";
}

void HttpConnectionHandler::onData(std::shared_ptr<tcpserver::Connection> connection, const std::string& data) {
    int sockfd = connection->getSocket();
    
    // Add new data to the connection's buffer
    if (connectionBuffers_.find(sockfd) == connectionBuffers_.end()) {
        connectionBuffers_[sockfd] = "";
    }
    
    // Append new data to the connection's buffer
    connectionBuffers_[sockfd] += data;
    
    // Check if we have a complete HTTP request
    std::string& buffer = connectionBuffers_[sockfd];
    if (isCompleteHttpRequest(buffer)) {
        processHttpRequest(connection, buffer);
        // Clear the buffer after processing
        buffer.clear();
    }
}

void HttpConnectionHandler::onDisconnect(std::shared_ptr<tcpserver::Connection> connection) {
    // Clean up connection buffer
    connectionBuffers_.erase(connection->getSocket());
}

void HttpConnectionHandler::processHttpRequest(std::shared_ptr<tcpserver::Connection> connection, const std::string& data) {
    // Parse HTTP request
    Request req;
    if (!HttpParser::parseRequest(data, req)) {
        // Send 400 Bad Request for malformed requests
        Response resp(400, "Bad Request", "Invalid HTTP request");
        connection->send(resp.to_string());
        return;
    }
    
    // Use the router to handle the request
    Response resp = router_.route(req);
    
    // Send response
    connection->send(resp.to_string());
}

bool HttpConnectionHandler::isCompleteHttpRequest(const std::string& data) {
    // Look for the double CRLF that indicates end of HTTP headers
    return data.find("\r\n\r\n") != std::string::npos;
}

