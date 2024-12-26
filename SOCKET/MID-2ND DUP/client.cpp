
/*#include<bits/stdc++.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<poll.h>
#include<arpa/inet.h>
#include<netinet/in.h>
using namespace std;

int Generate_Client(int port)
{
    int client_fd,status;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
     
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return client_fd;
}

int UDP_socket(int port)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    return sockfd;
}

int main()
{
    int port = 6734;
    char buffer[1024];

    
    while(1)
    {
        for(int i = 0; i < 5; i++)
        {
            int clfd;
            if(i % 2 != 0)
            {
                clfd = UDP_socket(port + i);
                if(clfd == -1){
                    continue;
                }
                 struct sockaddr_in servaddr;
                 socklen_t len ;
                // Prepare message to send
                std::string message = "Hello from client to UDP port " + std::to_string(port + i);
                
                // Send message to server
                sendto(clfd, message.c_str(), message.length(), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

                // Receive acknowledgment from server
                int n = recvfrom(clfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&servaddr, &len);
                if(n < 0){
                    perror("recvfrom failed");
                }
                else{
                    buffer[n] = '\0'; // Null-terminate the received string
                    cout << "Connectionless Server " << i << " : " << buffer << endl;
                }
                close(clfd);
            }
            else
            {
                clfd = Generate_Client(port + i);
                if(clfd == -1){
                    continue;
                }

                // Optionally, send data to the server if needed
                // For now, just receiving as per original code
                int n = recv(clfd, buffer, sizeof(buffer) - 1, 0);
                if(n <= 0){
                    //perror("recv failed");
                }
                else{
                    buffer[n] = '\0'; // Null-terminate the received string
                    cout << "Connection-oriented service: " << i+1 << " : " << buffer << endl;
                }
                close(clfd);
            }                    
        }
        sleep(3);
    }
    return 0;
}*/
