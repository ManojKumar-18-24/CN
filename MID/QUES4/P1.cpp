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
#define PORT 17000
#define PORT2 18000
int nsfd;
int nsfd2;

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

int Accept(int &sfd)
{
      struct pollfd fds[1];
      fds[0].fd=sfd;
      fds[0].events=POLLIN;
      while(1)
      {
            poll(fds,1,2000);
            if(fds[0].revents & POLLIN)
            {
                  cout<<"O to P polled\n";
                  struct sockaddr_in addr;
                  socklen_t len=sizeof(addr);
                  int nsfd=accept(sfd,(struct sockaddr*)&addr,&len);
                  return nsfd;                                                                  
            }
      }
      return -1;
}

void teaching()
{
      char buffer[]="Teaching from P1\0";
      send(nsfd,buffer,strlen(buffer),0);
}

void chatting()
{
      struct pollfd fds[1];
      fds[0].fd=nsfd2;
      fds[0].events=POLLIN;
      for(int i=0;i<3;i++)
 {     poll(fds,1,2000);
      if(fds[0].revents & POLLIN)
      {
            int r;
            recv(fds[0].fd,&r,sizeof(r),0);
            cout<<"received r= "<<r<<endl;
            sleep(1);
            int clfd=generate_client(r);
            send(clfd,"hi",2,0);
            close(clfd);
      }  }
}

int main()
{
        int sfd=co_server(PORT);
        nsfd=Accept(sfd);
        nsfd2=co_server(PORT2);
        nsfd2=Accept(nsfd2);
        pthread_t th[100];
        int i=0;
        while(1)
        {
                 int r=0;
                 recv(nsfd,&r,sizeof(r),0);
                 cout<<"received "<<r<<endl;
                 if(r==1)
                 {
                      teaching();
                 }
                 else 
                 {
                         chatting();
                        //pthread_create(&th[i],NULL,&chatting,NULL);
                       // pthread_join(th[i],NULL);
                 }
                 i++;
                 send(nsfd,&r,sizeof(r),0);
        }
        return 0;
}
