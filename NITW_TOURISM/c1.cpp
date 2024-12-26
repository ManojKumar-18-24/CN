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
#define PORT 12378

int generate_client(int port)
{
     	int sfd;
	struct sockaddr_in serv_addr;

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET,"127.0.0.1", &serv_addr.sin_addr);

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr))==-1)
	perror("\n connect : ");
	else printf("\nconnect succesful\n");
	return sfd;
}

int main()
{
     srand(time(0));
     int clfd=generate_client(PORT);
     char buffer[2];
     for(int i=0;i<5;i++)
     {
             recv(clfd,buffer,2,0);
             cout<<i<<" th tourist place: "<<buffer[0]<<buffer[1]<<" & location : ";
             recv(clfd,buffer,2,0);
             cout<<buffer[0]<<buffer[1]<<endl;
     }
     int r=rand()%5;
     send(clfd,&r,sizeof(int),0);
     cout<<r<<endl;
     char buf[100];
     while(1){int read=recv(clfd,buf,100,0);
     for(int i=0;i<read;i++)cout<<buf[i];}
     return 0;
}
