#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PROTOCOL_NUM 75

int main() {
    int rsfd;
    struct sockaddr_in dest_addr;
    const char *message = "available";

    // Create raw socket with custom protocol number
    rsfd = socket(AF_INET, SOCK_RAW, PROTOCOL_NUM);
    if (rsfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set destination address (loopback)
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);

    // Send raw data (no IP header included)
    if (sendto(rsfd, message, strlen(message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto failed");
        close(rsfd);
        exit(EXIT_FAILURE);
    }

    printf("Message sent: %s\n", message);
    close(rsfd);
    return 0;
}
