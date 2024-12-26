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

#define PORT 13455
int rsfd[3]; //-->int arsfd, frsfd, wrsfd;

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

int receive(int sockfd,int protocol)
{
    int rsfd;
    struct sockaddr_in dest_addr;
    const char *message = "item available";
    int srsfd = socket(AF_INET, SOCK_RAW, 78+protocol);
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);
    if(sendto(srsfd, message, strlen(message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        cout<<"send to failed\n";
    }
    char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    // Receive the packet
    int packet_size = recvfrom(sockfd, buffer, 4096, 0, (struct sockaddr *)&sender_addr, &addr_len);
    if (packet_size < 0)
    {
        perror("recvfrom failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Extract IP header
    struct iphdr *ip_header = (struct iphdr *)buffer;

    // Print source address
    printf("Received packet from: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));

    // Extract data (payload) from the packet (skip the IP header)
    char *data = (char *)(buffer + (ip_header->ihl * 4)); // IHL is in 32-bit words
    int data_len = packet_size - (ip_header->ihl * 4);    // Remaining data after the IP header

    // Print data if available
    if (data_len > 0)
    {
        printf("Data: %.*s\n", data_len, data);
    }
    else
    {
        printf("No data in packet\n");
    }
    return 0;
}

int main()
{
    srand(time(0));
    char buffer[1024];
    for (int i = 0; i < 3; i++)
        rsfd[i] = socket(AF_INET, SOCK_RAW, 75 + i);
    while (true)
    {
        int port = rand()%3;
        receive(rsfd[port],port);
        sleep(3);
        int clfd = generate_client(port + PORT);
        recv(clfd, buffer, 1024, 0);
        cout << "buffer= " << buffer << endl;
        sleep(6);
    }
    return 0;
}