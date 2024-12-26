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
#define PORT 15002
#define PORT2 5050

int sfd;
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
	else printf("\nconnect succesful");
	return sfd;
}

int co_server(int port)
{
        int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	int opt=1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	listen(sfd,10);

	cli_len=sizeof(cli_addr);
	return sfd;
}

void * routine(void* args)
{ 
      struct pollfd fds[1];
      fds[0].fd=sfd;
      fds[0].events=POLLIN;
      while(1)
      {
            poll(fds,1,2000);
            if(fds[0].revents & POLLIN)
            {
                  cout<<"Personal doubt\n";
                  struct sockaddr_in addr;
                  socklen_t len=sizeof(addr);
                  int nsfd=accept(sfd,(struct sockaddr*)&addr,&len);
                  char buffer[2];
                  recv(nsfd,buffer,2,0);
                  cout<<"answer: "<<buffer<<endl;   
                  break;                                                           
            }
      }
}

int main()
{
        int clfd=generate_client(PORT);
        sfd=co_server(PORT2);
        struct pollfd fds[1];
        fds[0].fd=clfd;
        fds[0].events=POLLIN;
        int i=0;
        pthread_t th[100];
        while(1)
        {
             poll(fds,1,2000);
             if(fds[0].fd & POLLIN)
             {
                    cout<<"got polled\n";
                   char buffer[16];
                   int bytes=recv(fds[0].fd,buffer,17,0);
                   for(int i=0;i<bytes;i++)cout<<buffer[i];cout<<endl;
                   if(buffer[0]=='A')return 0;
             }
             char buffer[]="doubt\n";
             int r=5050;
             send(clfd,&r,sizeof(int),0);
             pthread_create(&th[i],NULL,&routine,NULL);
             //send(clfd,buffer,strlen(buffer),0);
             i++;
             sleep(2);
        }
         for(int j=0;j<i;j++)
        {
            pthread_join(th[i],NULL);
        }
        return 0;
}
