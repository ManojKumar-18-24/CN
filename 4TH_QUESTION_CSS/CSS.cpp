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

#define PORT 12479
#define SONYPORT 13000
#define SONYOFFERS 13001
int msgid;
int snclfd,osnclfd;
bool T1=true;
int T1count=0;
int T2count=0;
vector<int> store_nsfd;

struct message
{
    long type;
    char buffer[15];
    int port;
};

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

void Bajaj(char buffer[15],int port)
{
    sleep(3);
    int clfd=generate_client(port);
    send(clfd,"From Bajaj Service...Hope You got serviced\n",43,0);
    if(T1==true)
    {
        T1count++;
        T1=false;
    }
    else
    {
        T2count++;
        T1=true;
    }
}

void allocate_service(char buffer[2],int nsfd)
{
    char buffer2[15];
    recv(nsfd,buffer2,15,0);
    cout<<"buffer2 "<<buffer2<<endl;
    int port;
    recv(nsfd,&port,sizeof(int),0);
    cout<<"port "<<port<<endl;
    message msg;
    for(int i=0;i<15;i++)
    {
        if(buffer[i]=='E')break;
        else msg.buffer[i]=buffer2[i];
    }
    msg.port=port;
    if (buffer[0] == 'S' && buffer[1] == 'N')
    {
        send(snclfd,buffer2,15,0);
        send(snclfd,&port,sizeof(int),0);
    }
    else if (buffer[0] == 'S' && buffer[1] == 'C')
    {
        msg.type=1;
        msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
    }
    else if (buffer[0] == 'L' && buffer[1] == 'G')
    {
        msg.type=2;
        msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
    }
    else
    {
        Bajaj(buffer2,port);
    }
}

void *Send_Offers(void* args)
{
    message msg;
    while(true)
    {
        msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),4,0);
        cout<<"discount percentage: "<<msg.port<<endl;
        for(auto &i:store_nsfd)
        {
            send(i,"5 discount\n",15,0);
        }
        sleep(1);
    }
}

void *Send_sony_offers(void* args)
{
    struct pollfd fds[1];
    fds[0].fd=osnclfd;
    fds[0].events=POLLIN;
    char buffer[1024];
    while(true)
    {
        poll(fds,1,2000);
        if(fds[0].revents & POLLIN)
        {
            recv(fds[0].fd,buffer,21,0);
            cout<<"offer from sony: "<<buffer<<endl;
            for(auto &i:store_nsfd)
            {
                send(i,buffer,21,0);
            }
        }
    }
}

int main()
{
    osnclfd=generate_client(SONYOFFERS);
    sleep(1);
    snclfd = generate_client(SONYPORT);
    pthread_t th,sony_offers;
    pthread_create(&th,NULL,Send_Offers,NULL);
    store_nsfd.resize(0);
    key_t key = ftok("queue", 65);
    msgid = msgget(key, 0644 | IPC_CREAT);
    struct pollfd fds[1];
    fds[0].fd = co_server(PORT);
    fds[0].events = POLLIN;
    struct sockaddr_in cli_addr;
    socklen_t addr_len = sizeof(cli_addr);
    pthread_create(&sony_offers,NULL,Send_sony_offers,NULL);
    while (true)
    {
        poll(fds, 1, 2000);
        if (fds[0].revents & POLLIN)
        {
            int nsfd = accept(fds[0].fd, (struct sockaddr *)&cli_addr, &addr_len);
            char buffer[2];
            recv(nsfd, buffer, 2, 0);
            cout<<"service: "<<buffer[0]<<buffer[1]<<endl;
            allocate_service(buffer,nsfd);
            store_nsfd.push_back(nsfd);
        }
    }
    return 0;
}