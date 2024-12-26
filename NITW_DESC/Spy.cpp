
/*
    NITW DESK
    22CSB0C23
    Samudrala Manojkumar
    Spy
*/
#include <bits/stdc++.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#define PORT 54000
using namespace std;
int counti = 0;
int rrsfd;
int clfd;
string receive_string(int rsfd, char *buffer, size_t buffer_size)
{
    sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);
    // Receive the message
    ssize_t received_bytes = recvfrom(rsfd, buffer, buffer_size, 0,
                                      (sockaddr *)&src_addr, &addr_len);
    if (received_bytes < 0)
    {
        perror("recvfrom failed");
    }
    else if (received_bytes > 0)
    {
        buffer[received_bytes] = '\0'; // Null-terminate the received data
        // Skip IP header if using raw sockets (for IPv4)
        char *payload = buffer + 20;
        std::cout << "Received message: " << payload << std::endl;
        std::cout << "From IP: " << inet_ntoa(src_addr.sin_addr) << std::endl;
        send(clfd,inet_ntoa(src_addr.sin_addr),strlen(inet_ntoa(src_addr.sin_addr)),0);
        // send_string(rrsfd,payload,"172.20.247.122");
        return payload;
    }
    else
    {
        std::cout << "No data received." << std::endl;
    }
    return "";
}

int generate_client(int port)
{
    int sfd;
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("\n socket");
    else
        printf("\n socket created successfully\n");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if (connect(sfd, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) == -1)
        perror("\n connect : ");
    else
        printf("\nconnect succesful");
    return sfd;
}

int main()
{
    int rrsfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    char buffer[1024];
    int i = 0;
    clfd = generate_client(PORT);
    while (i < 10)
    {
        receive_string(rrsfd, buffer, 1024);
        i++;
    }
    send(clfd,&counti,sizeof(int),0);
    return 0;
}