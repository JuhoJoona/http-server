#ifndef HTTP_CONNECTION_HANDLER_HPP
#define HTTP_CONNECTION_HANDLER_HPP

#include "../../lib/tcpserver/include/tcpserver/connection_handler.hpp"
#include "../include/http/router.hpp"
#include "../include/http/parser.hpp"
#include <memory>
#include <map>

class HttpConnectionHandler : public tcpserver::ConnectionHandler {
public:
    HttpConnectionHandler(Router& router);
    ~HttpConnectionHandler() override = default;
    
    void onConnection(std::shared_ptr<tcpserver::Connection> connection) override;
    void onData(std::shared_ptr<tcpserver::Connection> connection, const std::string& data) override;
    void onDisconnect(std::shared_ptr<tcpserver::Connection> connection) override;

private:
    void processHttpRequest(std::shared_ptr<tcpserver::Connection> connection, const std::string& data);
    bool isCompleteHttpRequest(const std::string& data);
    
    Router& router_;
    std::map<int, std::string> connectionBuffers_;
};

#endif // HTTP_CONNECTION_HANDLER_HPP
