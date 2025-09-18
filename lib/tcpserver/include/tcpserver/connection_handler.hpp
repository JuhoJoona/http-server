#ifndef CONNECTION_HANDLER_HPP
#define CONNECTION_HANDLER_HPP

#include <memory>
#include <string>

namespace tcpserver {

class Connection;

/**
 * Abstract interface for handling incoming TCP connections
 */
class ConnectionHandler {
public:
    virtual ~ConnectionHandler() = default;
    
    /**
     * Called when a new connection is established
     * @param connection Shared pointer to the connection object
     */
    virtual void onConnection(std::shared_ptr<Connection> connection) = 0;
    
    /**
     * Called when data is received on a connection
     * @param connection Shared pointer to the connection object
     * @param data The received data
     */
    virtual void onData(std::shared_ptr<Connection> connection, const std::string& data) = 0;
    
    /**
     * Called when a connection is closed
     * @param connection Shared pointer to the connection object
     */
    virtual void onDisconnect(std::shared_ptr<Connection> connection) = 0;
};

} // namespace tcpserver

#endif // CONNECTION_HANDLER_HPP
