/*
    NITW DESK
    22CSB0C23
    Samudrala Manojkumar
    MNC
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
using namespace std;
#define PORT 54000
int srsfd, rrsfd;

void send_string(int rsfd, string message, const char *destination_ip)
{
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(destination_ip);
    // Send the message
    if (sendto(rsfd, message.c_str(), message.length() + 1, 0, (sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto failed");
    }
    else
    {
        std::cout << "Message sent successfully" << std::endl;
    }
}

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
        return payload;
    }
    else
    {
        std::cout << "No data received." << std::endl;
    }
    return "";
}

void *send_question(void *args)
{
    int i=0;
    while (i<10)
    {
        send_string(srsfd, "question\n", "172.20.247.122");
        sleep(3);
        i++;
    }
}

void *recv_question(void *args)
{
    char buffer[1024];
    int i=0;
    while (i<10)
    {
        receive_string(rrsfd, buffer, sizeof(buffer));
        i++;
    }
}

int co_server(int port)
{
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("\n socket ");
    else
        printf("\n socket created successfully");
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        perror("\n bind : ");
    else
        printf("\n bind successful ");
    listen(sfd, 10);
    return sfd;
}

void* tcp(void* args)
{
    int sfd=co_server(PORT);
    struct sockaddr_in cli_addr;
    socklen_t cli_len=sizeof(cli_addr);
    int nsfd=accept(sfd,(struct sockaddr*)&cli_addr,&cli_len);
    while(1){char buffer[1024];
    recv(nsfd,buffer,1024,0);
    cout<<"buffer= "<<buffer<<endl; 
    }
}

int main()
{
    srsfd = socket(AF_INET, SOCK_RAW, 75);
    rrsfd = socket(AF_INET, SOCK_RAW, 76);
    pthread_t th[3];
    pthread_create(&th[0], NULL, send_question, NULL);
    pthread_create(&th[1], NULL, recv_question, NULL);
    pthread_create(&th[2],NULL,tcp,NULL);
    while (1)
        ;
    return 0;
}