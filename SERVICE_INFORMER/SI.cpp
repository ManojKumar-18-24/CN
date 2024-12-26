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
#define PORT 8096   /*clients connect to this port*/
#define PORT2 9045  /*server sends their port numbers trrough this*/

int sfd;

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

	listen(sfd,100);

	return sfd;
}

void send_vector(int socket_fd, vector<int>& vec) {
    int size = vec.size();
    
    // Send the size of the vector first
    send(socket_fd, &size, sizeof(size), 0);
    
    // Send the vector data
    send(socket_fd, vec.data(), size * sizeof(int), 0);
}

void * s0_service(void * args)
{
      struct pollfd fds[1];
      fds[0].fd=sfd;
      fds[0].events=POLLIN;
      while(1)
      {
            poll(fds,1,2000);
            if(fds[0].fd & POLLIN)
            {
                    cout<<"someone requested for S0"<<endl;
                    struct sockaddr_in cli_addr;
                    socklen_t cli_len=sizeof(cli_addr);
                    int nsfd=accept(fds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);
                    send(nsfd,"Service_from_S0\0",16,0);
                    close(nsfd);
            }
            sleep(1);
      }
}

int main()
{
   sfd=co_server(12332);
   pthread_t th=6;
   pthread_create(&th,NULL,&s0_service,NULL);
   vector<int> ports={12332};
   int csfd=co_server(PORT);
   int sifd=co_server(PORT2);
   struct pollfd fds[1];
   struct pollfd cfds[1];
   cfds[0].fd=csfd;
   cfds[0].events=POLLIN;
   fds[0].fd=sifd;
   fds[0].events=POLLIN;
   while(1)
   {
         poll(fds,1,2000);
         if(fds[0].revents & POLLIN)
         {
              int r;
              struct sockaddr_in cli_addr;
              socklen_t cli_len=sizeof(cli_addr);
              int nsfd=accept(fds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);             
              recv(nsfd,&r,sizeof(r),0);
              cout<<"received_port= "<<r<<endl;
              ports.push_back(r);
              close(nsfd);
         }
         poll(cfds,1,2000);
         if(cfds[0].revents & POLLIN)
         {
              struct sockaddr_in cli_addr;
              socklen_t cli_len=sizeof(cli_addr);
              int nsfd=accept(cfds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);
              send_vector(nsfd,ports);
              cout<<"\nvector_sent\n";
              close(nsfd);
         }
         sleep(1);
   }
   pthread_join(th,NULL);
   return 0;
}
