#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.137.242"  // Replace with the server's IP address if running on different machine
#define SERVER_PORT 13245

int main() {
    int sock;
    struct sockaddr_in server_address;

    // Set up server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        printf("Invalid address or Address not supported\n");
        return -1;
    }

    // Loop to attempt connections
    while (1) {
        // Create socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            return -1;
        }

        // Attempt to connect
        printf("Attempting to connect to %s:%d...\n", SERVER_IP, SERVER_PORT);
        if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
            perror("Connection failed (expected if server is not accepting)");
        } else {
            printf("Connected to the server!\n");
            close(sock);// Exit if the connection was actually accepted (not expected here)
        }

        close(sock);
        sleep(1);  // Retry after a delay
    }

    return 0;
}
