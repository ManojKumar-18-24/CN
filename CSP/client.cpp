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
using namespace std;
#define PORT 12035
#define PROTOCOL_NUM 75

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

void raw_service()
{
    int sockfd;
    char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, PROTOCOL_NUM);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening for custom protocol packets...\n");
    // Receive the packet
    ssize_t data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
    if (data_size < 0) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }
    // Extract IP header
    struct ip *ip_header = (struct ip *)buffer;
    //struct iphdr *ip_header = (struct iphdr *)buffer;
    char *data = buffer + ip_header->ip_hl * 4;  // IP header length in 32-bit words
    printf("Received packet from: %s\n", inet_ntoa(sender_addr.sin_addr));
    printf("Data: %s\n", data);
}

int main()
{
    srand(time(0));
    while(1)
    {
        int clfd=generate_client(PORT);
        int r=rand()%5+1;
        send(clfd,&r,sizeof(r),0);
        send(clfd,"172.20.188.119\n",15,0);
        char buffer[256];
        r=recv(clfd,buffer,256,0);
        if(buffer[0]=='r')
        {
            raw_service();
        }
        else
        {
            for(int i=0;i<r;i++)cout<<buffer[i];cout<<endl;
        } 
        cout<<endl;
        close(clfd);
        sleep(2);
    }
    return 0;
}