#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include <cstdlib>

#define NUM_SOCKETS 2  // Number of sockets (ports) on server

int ports[] = {8080, 8081};  // Server will listen on these ports

// Function to handle communication on each socket
void* handle_connection(void* new_socket_ptr) {
    int new_socket = *(int*)new_socket_ptr;
    char buffer[1024] = {0};
    std::string response = "Response from server";

    // Read message from client
    read(new_socket, buffer, 1024);
    std::cout << "Received: " << buffer << std::endl;

    // Send response back to client
    send(new_socket, response.c_str(), response.length(), 0);
    std::cout << "Response sent" << std::endl;

    // Close the socket
    close(new_socket);
    delete (int*)new_socket_ptr;
    return nullptr;
}

int main() {
    int server_fds[NUM_SOCKETS];  // Array to hold socket file descriptors
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    // Create multiple sockets, each listening on different ports
    for (int i = 0; i < NUM_SOCKETS; i++) {
        if ((server_fds[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        int opt = 1;
        if (setsockopt(server_fds[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(ports[i]);

        // Bind the socket to the address and port
        if (bind(server_fds[i], (struct sockaddr*)&address, sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        // Listen for incoming connections
        if (listen(server_fds[i], 3) < 0) {
            perror("listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Server listening on port " << ports[i] << std::endl;
    }

    // Infinite loop to accept connections on all sockets
    while (true) {
        for (int i = 0; i < NUM_SOCKETS; i++) {
            int* new_socket = new int;
            if ((*new_socket = accept(server_fds[i], (struct sockaddr*)&address, &addrlen)) < 0) {
                perror("accept failed");
                delete new_socket;
                exit(EXIT_FAILURE);
            }

            // Create a new thread to handle the connection
            pthread_t thread_id;
            pthread_create(&thread_id, nullptr, handle_connection, new_socket);
            pthread_detach(thread_id);  // Detach the thread to handle client independently
        }
    }

    // Close all server sockets
    for (int i = 0; i < NUM_SOCKETS; i++) {
        close(server_fds[i]);
    }

    return 0;
}

