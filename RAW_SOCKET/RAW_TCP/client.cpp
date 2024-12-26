#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Create a standard TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Define server address (IP and Port)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Must match the port in the raw packet
    server_addr.sin_addr.s_addr = inet_addr("10.0.2.15"); // Server's IP

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Receive message from the server
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    close(sockfd);
    return 0;
}

