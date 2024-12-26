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

#define PORT 13457
#define PROTOCOL_NUM 77

bool stock_available=true;
struct pollfd fds[1];

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

void *Enquiry(void* args)
{
    int rsfd;
    struct sockaddr_in dest_addr;
    const char *message = "available";
    const char *message2= "not available";
    // Create raw socket with custom protocol number
    rsfd = socket(AF_INET, SOCK_RAW, PROTOCOL_NUM);
    if (rsfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    int crsfd=socket(AF_INET,SOCK_RAW,PROTOCOL_NUM+3);
    // Set destination address (loopback)
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    char buffer[4096];
    while(1){
    // Send raw data (no IP header included)
    int packet_size = recvfrom(crsfd, buffer, 4096, 0, (struct sockaddr *)&sender_addr, &addr_len);
    sleep(2);
    if (stock_available && sendto(rsfd, message, strlen(message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto failed");
        close(rsfd);
        exit(EXIT_FAILURE);
    }
    else {
        if(!stock_available && sendto(rsfd, message2, strlen(message2), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
        {
            perror("sendto failed");
            close(rsfd);
            exit(EXIT_FAILURE);
        }
    }

    printf("Message sent: %s\n", message);
    sleep(5);
    }
}

int main()
{
    pthread_t th;
    pthread_create(&th,NULL,Enquiry,NULL);
    struct sockaddr_in cli_addr;
    socklen_t cli_len=sizeof(cli_addr);
    while (true)
    {
        stock_available=true;
        fds[0].fd = co_server(PORT);
        fds[0].events = POLLIN;
        int i = 0;
        while (i < 2)
        {
            poll(fds, 1, 2000);
            if (fds[0].revents & POLLIN)
            {
                i++;
                int nsfd=accept(fds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);
                send(nsfd,"Walmart delivered an item\n",50,0);
                close(nsfd);
            }
        }
        cout<<"closed\n";
        stock_available=false;
        close(fds[0].fd);
        sleep(20);
        break;
    }
    while(1);
    return 0;
}