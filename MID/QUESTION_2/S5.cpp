#include<bits/stdc++.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<poll.h>
#include<arpa/inet.h>
#include<netinet/in.h>
using namespace std;

void Init_CO_socket(int &server_fd,int port)
{
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void Serve(int nsfd)
{
       send(nsfd,"Service_from_S5\n",16,0);
}

int main(int argc,char * argv[])
{
      int port=atoi(argv[1]);
      int sfd;
      Init_CO_socket(sfd,port);
      struct pollfd fds[1];
      fds[0].fd=sfd;
      fds[0].events=POLLIN;
      nfds_t nfs=1;
      struct sockaddr_in address;
      socklen_t len;
      while(1)
      {
            poll(fds,nfs,2000);
            if(fds[0].revents & POLLIN)
            {
                    int nsfd=accept(sfd,(struct sockaddr*)&address,&len);
                    int c=0;
                     c=fork();
                     if(c==0)
                     {
                           close(sfd);
                           Serve(nsfd);
                           return 0;
                     }
                     close(nsfd);
            }
      } 
      return 0;
}
