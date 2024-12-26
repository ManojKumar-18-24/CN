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
#define PORT 12000
#define TCP_PORT 12035

void send_ip(string ip,int port_no)
{
    int sfd;
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        perror("\n socket");
    else
        printf("\n socket created successfully\n");
    socklen_t serv_len = sizeof(serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    sendto(sfd, ip.c_str(), ip.length(), 0, (struct sockaddr *)&serv_addr, serv_len);
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

void *serve_client(void *args)
{
    int nsfd=*(int*)args;
    cout<<nsfd<<endl;
    int r;
    recv(nsfd,&r,sizeof(r),0);
    char buffer[256];
    recv(nsfd,buffer,sizeof(buffer),0);
    cout<<buffer<<endl;
    if(r==3)
    {
        int c=0;
        c=fork();
        if(c==0)
        {
            dup2(nsfd,0);
            dup2(nsfd,1);
            execl("./s3","./s3",(char*)NULL);
        }
    }
    else if(r==5)
    {
        int c=0;
        c=fork();
        if(c==0)
        {
            dup2(nsfd,0);
            dup2(nsfd,1);
            execl("./s5","./s5",(char*)NULL);
        }       
    }
    else
    {
        send(nsfd,"r\n",2,0);
        sleep(2);
        send_ip(buffer,PORT+r);
    }
    close(nsfd);
}

int main()
{
    struct pollfd fds[1];
    fds[0].fd=co_server(TCP_PORT);
    fds[0].events=POLLIN;
    struct sockaddr_in cli_addr;
    socklen_t cli_len=sizeof(cli_addr);
    pthread_t th[100];
    int i=0;
    while(1)
    {
        poll(fds,1,2000);
        if(fds[0].revents & POLLIN)
        {
            int nsfd=accept(fds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);
            int *ptr=&nsfd;
            pthread_create(&th[i],NULL,&serve_client,(void*)ptr);
            i++;
        }
        sleep(1);
    }
    return 0;
}