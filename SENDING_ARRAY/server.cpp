#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

void send_array(int socket_fd, int *array, int array_size) {
    send(socket_fd, &array_size, sizeof(array_size), 0);
    send(socket_fd, array, sizeof(int) * array_size, 0);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen for connections
    listen(server_fd, 3);

    // Accept a connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    // Prepare array to send
    int array[] = {10, 20, 30, 40, 50};
    int array_size = sizeof(array) / sizeof(array[0]);

    // Send the array
    send_array(new_socket, array, array_size);

    close(new_socket);
    close(server_fd);
    return 0;
}

