#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void recv_array(int socket_fd, int *array, int *array_size) {
    recv(socket_fd, array_size, sizeof(*array_size), 0);
    recv(socket_fd, array, sizeof(int) * (*array_size), 0);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int array[10];
    int array_size = 0;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Connect to server
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // Receive the array
    recv_array(sock, array, &array_size);

    // Print the received array
    std::cout << "Received array: ";
    for (int i = 0; i < array_size; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;

    close(sock);
    return 0;
}

