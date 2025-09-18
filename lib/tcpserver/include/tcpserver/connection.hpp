#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <memory>
#include <string>
#include <sys/socket.h>

namespace tcpserver {

class EventLoop;

/**
 * Represents a TCP connection
 */
class Connection {
public:
    Connection(int sockfd, EventLoop& eventLoop);
    ~Connection();

    /**
     * Send data to the client
     * @param data The data to send
     * @return true if successful, false otherwise
     */
    bool send(const std::string& data);
    
    /**
     * Send data to the client
     * @param data The data to send
     * @param length Length of the data
     * @return true if successful, false otherwise
     */
    bool send(const char* data, size_t length);
    
    /**
     * Close the connection
     */
    void close();
    
    /**
     * Check if the connection is closed
     * @return true if closed, false otherwise
     */
    bool isClosed() const;
    
    /**
     * Get the socket file descriptor
     * @return The socket file descriptor
     */
    int getSocket() const { return sockfd_; }
    
    /**
     * Get the client's IP address
     * @return The client's IP address as a string
     */
    std::string getClientAddress() const;
    
    /**
     * Get the client's port
     * @return The client's port number
     */
    int getClientPort() const;
    
    /**
     * Get the read buffer content
     * @return The content of the read buffer
     */
    std::string getReadBuffer() const;
    
    /**
     * Clear the read buffer
     */
    void clearReadBuffer();

private:
    friend class TcpServer;
    
    void handleRead();
    void handleWrite();
    void registerWithEventLoop();
    void unregisterFromEventLoop();
    
    int sockfd_;
    EventLoop& eventLoop_;
    bool closed_;
    std::string clientAddress_;
    int clientPort_;
    std::string readBuffer_;
    std::string writeBuffer_;
};

} // namespace tcpserver

#endif // TCP_CONNECTION_HPP
