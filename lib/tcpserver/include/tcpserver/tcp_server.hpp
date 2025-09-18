#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include "connection_handler.hpp"
#include "connection.hpp"
#include <memory>
#include <map>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>

namespace tcpserver {

class EventLoop;

/**
 * High-performance TCP server with event-driven I/O
 */
class TcpServer {
public:
    /**
     * Constructor
     * @param port The port to listen on
     * @param handler The connection handler for processing connections
     */
    TcpServer(int port, std::shared_ptr<ConnectionHandler> handler);
    
    /**
     * Destructor
     */
    ~TcpServer();
    
    /**
     * Start the server (blocking call)
     */
    void start();
    
    /**
     * Stop the server
     */
    void stop();
    
    /**
     * Check if the server is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running_; }
    
    /**
     * Get the port the server is listening on
     * @return The port number
     */
    int getPort() const { return port_; }

private:
    void eventLoop();
    void handleNewConnection();
    void handleClientEvent(int clientFd, int events);
    
    int port_;
    int sockfd_;
    bool running_;
    std::shared_ptr<ConnectionHandler> handler_;
    std::unique_ptr<EventLoop> eventLoop_;
    std::map<int, std::shared_ptr<Connection>> connections_;
    
    // Server socket address
    struct sockaddr_in address_;
};

} // namespace tcpserver

#endif // TCP_SERVER_HPP
