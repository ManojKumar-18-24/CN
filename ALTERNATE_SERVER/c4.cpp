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

#define PORT1 12000
#define PORT2 12001

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

int main()
{
      int clfd=generate_client(PORT1);
      int aclfd=generate_client(PORT2);
      struct pollfd sc[1];
      sc[0].fd=clfd;
      sc[0].events=POLLIN;
      struct pollfd sac[1];
      sac[0].fd=aclfd;
      sac[0].events=POLLIN;
      int server=true;
      while(1)
      {
            if(server)
            {
                     send(clfd,"Change_this_output\0",19,0);
                     while(1)
                     {
                            poll(sc,1,2000);
                            if(sc[0].revents & POLLIN)
                            {
                                   char buffer[50];
                                   int read=recv(sc[0].fd,buffer,50,0);
                                   for(int i=0;i<read;i++)cout<<buffer[i];cout<<endl;
                                   if(buffer[0]=='n')server=false;
                                   break;
                            }
                     }
            }
            else
            {     
                 poll(sc,1,2000);
                  if(sc[0].revents & POLLIN)
                  {
                            char buffer[50];
                            int read=recv(sc[0].fd,buffer,50,0);
                            for(int i=0;i<read;i++)cout<<buffer[i];cout<<endl;
                            if(buffer[0]=='s')server=true;
                  } 
                  else 
                  {   
                         send(aclfd,"Change_this_output\0",19,0);
                         while(1)
                         {
                               poll(sac,1,2000);
                               if(sac[0].revents & POLLIN)
                               {
                                    char buffer[50];
                                    int read=recv(sac[0].fd,buffer,50,0);
                                    for(int i=0;i<read;i++)cout<<buffer[i];cout<<endl;
                                    break;
                               }
                          }
                   }             
            }
      }
      return 0;
}
