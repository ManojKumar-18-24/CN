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

int msgid;

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

void* Give_Offers(void * args)
{
    message msg;
    msg.type=4;
    msg.port=5;//here it is discount percentage....
    while(true)
    {
        sleep(10);
        msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
    }
}

int main()
{
    pthread_t th;
    pthread_create(&th,NULL,Give_Offers,NULL);
    message msg;
    while(true)
    {
        msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),1,0);
        cout<<"port= "<<msg.port<<endl;
        cout<<"ip= "<<msg.buffer<<endl;
        sleep(3);
        int clfd=generate_client(msg.port);
        send(clfd,"You are getting serviced from Samsung\n",38,0);
        sleep(2);
    }
    return 0;
}