#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT_1 8080
#define PORT_2 8081

void connect_to_server(int port) {
    int client_fd;
    struct sockaddr_in serv_addr;
    std::string message = "Hello from client";
    char buffer[1024] = {0};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return;
    }

    // Send message to server
    send(client_fd, message.c_str(), message.length(), 0);
    std::cout << "Message sent to server on port " << port << std::endl;

    // Read server's response
    read(client_fd, buffer, 1024);
    std::cout << "Response from server on port " << port << ": " << buffer << std::endl;

    // Close the socket
    close(client_fd);
}

int main() {
    // Connect to server on multiple ports
    connect_to_server(PORT_1);  // Connect to port 8080
    connect_to_server(PORT_2);  // Connect to port 8081

    return 0;
}

