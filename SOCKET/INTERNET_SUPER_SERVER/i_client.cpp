#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

//mutiple clients == execute as
//  ./client 0
//  ./client 1
//  ./client 2

int main(int argc, char *argv[]) 
{
    char ch = argv[1][0];
    int port_add = ch - '0';
    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    char message[] = "Hello from client";

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080+port_add);
    
    // Convert server IP address from text to binary form
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("\nConnection Faileddddd\n");
        return -1;
    }

    // Send message to server
    send(sock, message, strlen(message), 0);
    printf("Message sent: %s\n", message);

    // Receive response from server
    read(sock, buffer, 1024);
    printf("Response from server: %s\n", buffer);

    // Close socket
    close(sock);

    return 0;
}
