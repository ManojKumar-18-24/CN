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

#define PORT 12000

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

void* service(void* args)
{
         int  nsfd=*(int *)args;
         free(args);
         cout<<"nsfd in thread: "<<nsfd<<endl;
         int i=0;
         while(1)
         {
              cout<<i<<endl;
              if(i%2==0)
              {
                       int c=0;
                       c=fork();
                       if(c==0)
                       {
                               dup2(nsfd,1);
                               dup2(nsfd,0);
                               if(execl("./service","./service","S",NULL)==-1)perror("exec failed\n");
                       }
                       //wait();
              }
              else
              {
                       send(nsfd,"not_serving",11,0);
                       sleep(5);
                       send(nsfd,"serving",7,0);
              }  
              i++;
              sleep(2);
         }
}

int main()
{
      pthread_t th[10];
      int sfd=co_server(PORT);
      struct pollfd fds[1];
      fds[0].fd=sfd;
      fds[0].events=POLLIN;
      int i=0;
      while(1)
      {
             poll(fds,1,2000);
             if(fds[0].revents & POLLIN)
             {
                   cout<<"polled\n";
                   struct sockaddr_in addr;
                   socklen_t len=sizeof(addr);
                   int* nsfd_ptr = (int*)malloc(sizeof(int));

            // Accept new connection and store the socket descriptor in nsfd_ptr
                   *nsfd_ptr = accept(sfd, (struct sockaddr*)&addr, &len);
                   cout<<"nsfd in for loop= "<<*nsfd_ptr<<endl;
                   pthread_create(&th[i],NULL,&service,nsfd_ptr);
                   i++;
             }
             sleep(2);
      }
      return 0;
}

