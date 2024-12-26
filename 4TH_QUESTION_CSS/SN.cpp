#include<bits/stdc++.h>
#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<poll.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<semaphore.h>
using namespace std;

#define SONYPORT 13000

int co_server(int port)
{
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len=sizeof(cli_addr);
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
    int nsfd=accept(sfd,(struct sockaddr*)&cli_addr,&cli_len);
    return nsfd;
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

void *Give_Offers(void *args)
{
    int nsfd=*(int *)args;
    free(args);
    while(true)
    {
        sleep(10);
        send(nsfd,"5 discount from Sony\n",23,0);
    }
}

int main()
{    
    pthread_t th;
    int nsfd=co_server(SONYPORT+1);
    struct pollfd fds[1];
    fds[0].fd=co_server(SONYPORT);
    fds[0].events=POLLIN;
    int *a=(int*)malloc(sizeof(int));
    *a=nsfd;
    pthread_create(&th,NULL,Give_Offers,a);
    char buffer[15];
    int port;
    while(true)
    {
        poll(fds,1,2000);
        if(fds[0].revents & POLLIN)
        {
            recv(fds[0].fd,buffer,15,0);
            cout<<"buffer= "<<buffer<<endl;
            recv(fds[0].fd,&port,sizeof(int),0);
            cout<<"port= "<<port<<endl;
            sleep(3);
            int clfd=generate_client(port);
            send(clfd,"You are getting serviced from Sony\n",35,0);
        }
    }
    return 0;
}