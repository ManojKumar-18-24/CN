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
using namespace std;

#define PORT 12345

string msgs[4]={"a 32245 f1.txt\n","p f2.txt\n","a 32245 f2.txt\n","p f1.txt\n"};

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
    inet_pton(AF_INET, "192.168.137.242", &serv_addr.sin_addr);
    if (connect(sfd, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) == -1)
        {perror("\n connect : "); return -1;}
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

void get_data(int nsfd)
{
    char buffer[1024];
    int read;
    while(true)
    {
        read=recv(nsfd,buffer,1024,0);
        for(int i=2;i<read;i++)
        {
            cout<<buffer[i]<<" ";
            if(buffer[i]=='d' && buffer[i-1]=='n' && buffer[i-2]=='e')return;
        }
    }
}

void * get_service(void *args)
{
    int clfd=*(int*)args;
    cout<<"clfd= "<<clfd<<endl;
    int r=rand()%4;
    cout<<"sent string by clfd "<<clfd<<" is "<<msgs[r]<<endl;
    free(args);
    send(clfd,msgs[r].c_str(),msgs[r].length(),0);
    int port=32245;
    if(r%2!=0)
    {
        recv(clfd,&port,sizeof(int),0);
        sleep(5);
        int pclfd=generate_client(port);
        cout<<"port= "<<port<<endl;
        cout<<"pclfd= "<<pclfd<<endl;
        get_data(pclfd);
    }
    else
    {
        int nsfd=co_server(port);
        get_data(nsfd);
    }
    return (void*)5;
}

int main()
{
    srand(time(0));
    pthread_t th[100];
    int thread_no = 0;
    while(true)
    {
        int clfd=generate_client(PORT);
        if(clfd==-1)continue;
        int *a = (int *)malloc(sizeof(int));
        *a=clfd;
        pthread_create(&th[thread_no++], NULL,get_service, a);
        pthread_join(th[thread_no-1],NULL);
        sleep(10);
    }
}