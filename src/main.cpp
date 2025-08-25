#include <sys/socket.h>
#include <netinet/in.h>   // sockaddr_in, htons, htonl
#include <arpa/inet.h>    // inet_pton, inet_ntoa
#include <unistd.h>       // close()
#include <iostream>

int main() {

    // create socket 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    //set socket options
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //bind socket to ip and port
    sockaddr_in address;
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); 

    if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(sockfd);
        return 1;
    }

    //listen for incoming connections
    if(listen(sockfd, 10) == -1) {
        std::cerr << "Failed to listen for incoming connections" << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Server is listening on port 8080" << std::endl;

    //accept incoming connections
    sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int client_fd = accept(sockfd, (struct sockaddr *)&client_address, &client_address_len);
    if(client_fd == -1) {
        std::cerr << "Failed to accept incoming connection" << std::endl;
        close(sockfd);
        return 1;   
    }

    std::cout << "Accepted incoming connection from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << std::endl;
    

    return 0;
}