#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>

#define PORT 13245
#define BACKLOG 5

int main() {
    int server_fd;
    struct sockaddr_in address;
    struct pollfd pfd[1];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Set up poll on the server socket
    pfd[0].fd = server_fd;
    pfd[0].events = POLLIN;

    while (1) {
        int poll_count = poll(pfd, 1, 2000);
        if (poll_count < 0) {
            perror("Poll failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if (pfd[0].revents & POLLIN) {
            printf("Incoming connection detected (not accepting)...\n");
        }
    }
    close(server_fd);
    return 0;
}
