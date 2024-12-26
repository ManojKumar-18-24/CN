#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Server address setup
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(sockfd);
        return -1;
    }

    int len, n;
    len = sizeof(cliaddr); // Length of client address

    // Receive message from client
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, (socklen_t *)&len);
    buffer[n] = '\0'; // Null-terminate the received string
    std::cout << "Client: " << buffer << std::endl;

    // Send acknowledgment to client
    const char *ack = "Message received";
    sendto(sockfd, ack, strlen(ack), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
    std::cout << "Acknowledgment sent to client" << std::endl;

    // Close the socket
    close(sockfd);
    return 0;
}
