#include<bits/stdc++.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

int Generate_Client(int port)
{
    int client_fd, status;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n TCP Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
     
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nTCP Connection Failed \n");
        return -1;
    }
    return client_fd;
}

int UDP_socket()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "UDP Socket creation failed" << std::endl;
        return -1;
    }

    return sockfd;
}

int main()
{
    int port = 6734;
    char buffer[1024];
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);
    
    while(1)
    {
        for(int i = 0; i < 5; i++)
        {
            int clfd;
            if(i % 2 != 0) 
            {
                clfd = UDP_socket();
                if(clfd == -1){
                    continue;
                }

                memset(&servaddr, 0, sizeof(servaddr));
                servaddr.sin_family = AF_INET; 
                servaddr.sin_port = htons(port + i);
                
                if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
                    std::cerr << "Invalid server address/ Address not supported" << std::endl;
                    close(clfd);
                    continue;
                }

                std::string message = "Hello from client to UDP port " + std::to_string(port + i);
               
                ssize_t sent_bytes = sendto(clfd, message.c_str(), message.length(), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
                if(sent_bytes < 0){
                    perror("UDP sendto failed");
                    close(clfd);
                    continue;
                }

                ssize_t n = recvfrom(clfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&servaddr, &len);
                if(n < 0){
                    perror("UDP recvfrom failed");
                }
                else{
                    buffer[n] = '\0'; 
                    cout << "Connectionless Server " << i << " : " << buffer << endl;
                }
                close(clfd);
            }
            else // TCP
            {
                clfd = Generate_Client(port + i);
                if(clfd == -1){
                    continue;
                }

                int n = recv(clfd, buffer, sizeof(buffer) - 1, 0);
                if(n <= 0){
                    // Connection closed or error
                    // Optionally handle errors here
                }
                else{
                    buffer[n] = '\0'; 
                    cout << "Connection-oriented service: " << i << " : " << buffer << endl;
                }
                close(clfd);
            }                    
        }
        sleep(3);
    }
    return 0;
}
