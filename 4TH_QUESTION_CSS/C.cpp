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

#define PORT 12479

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

string arr[]={"SN","BA","SC","LG"};

void *Receive_Offers(void* args)
{
    int clfd=*(int*)args;
    char buffer[1024];
    while(true)
    {
        recv(clfd,buffer,1024,0);
        cout<<"offer: "<<buffer<<endl;
        sleep(1);
    }
}

int main()
{
    srand(time(0));
    int clfd=generate_client(PORT);
    int r=rand()%4;
    cout<<"requested for "<<arr[r]<<endl;
    send(clfd,arr[r].c_str(),2,0);
    send(clfd,"172.20.147.234\n",15,0);
    int port=12000+(rand()%1000);
    cout<<"port= "<<port<<endl;
    send(clfd,&port,sizeof(int),0);
    pthread_t th;
    int *a=(int*)malloc(sizeof(int));
    *a=clfd;
    pthread_create(&th,NULL,Receive_Offers,a);
    int get_service=co_server(port);
    char buffer[50];
    recv(get_service,buffer,50,0);
    cout<<"buffer= "<<buffer<<endl;
    while(1);
    return 0;
}