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
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<poll.h>
using namespace std;
#define PORT  15467
#define PORT2 8091

int Generate_Client()
{
	int sfd;
	struct sockaddr_in serv_addr;

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT2);
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET,"127.0.0.1", &serv_addr.sin_addr);

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr))==-1)
	perror("\n connect : ");
	else printf("\nconnect succesful");
	return sfd;
}

int CO_server(int port)
{
  	int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;
	//int port_no=atoi(port);

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
	sleep(3);
         struct pollfd fds[1];
         fds[0].fd=sfd;
         fds[0].events=POLLIN;
         while(1)
         {
              poll(fds,1,5000);
              if(fds[0].revents & POLLIN)
              {
                   int nsfd=accept(sfd,(struct sockaddr*)&cli_addr,&cli_len);
                   cout<<"nsfd_got\n";
                   return nsfd;
              }
         }
	return sfd;
}

int generate_client()
{
         int sfd;
	struct sockaddr_in serv_addr;

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET,"127.0.0.1", &serv_addr.sin_addr);

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr))==-1)
	perror("\n connect : ");
	else printf("\nconnect succesful");
	return sfd;
}

int main()
{
   int bat_fd=CO_server(PORT);
   int clfd=CO_server(PORT2);
   struct pollfd fds[1];
   fds[0].fd=bat_fd;
   fds[0].events=POLLIN;
   while(1)
   {
         poll(fds,1,5000);
         if(fds[0].revents & POLLIN)
         {
               //char buffer[2];
               int r;
               int ret=recv(fds[0].fd,&r,sizeof(r),0);
               cout<<"number received from batsman= "<<r<<endl;
               if(r==-1)
               {
                     cout<<"Ayyo..Batsman got out\n";
                     return 0;
               }
               if(r<=10)
               {
                      send(clfd,&r,sizeof(r),0);
                      cout<<"msg sent to umpire is "<<r<<endl;
               }
         }
         sleep(2);
         cout<<"waiting_for_next_iteration\n";
   }
   return 0;
}
