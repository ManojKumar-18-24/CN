// Server side C program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include<poll.h>
#include <unistd.h>
#define PORT 7869

void Telecast(int nsfd)
{
   char buffer[]="live telecat for livetelecaster\0";
   for(int i=0;i<5;i++)
   {
         send(nsfd,buffer,strlen(buffer),0);
         sleep(1);
    }
}

int main(int argc, char const* argv[])
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    
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
    address.sin_port = htons(PORT);

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
    struct pollfd fds[1];
    fds[0].fd=server_fd;
    while(1)
    {
          poll(fds,1,1000);
          if(fds[0].revents & POLLIN)
          {
                  int nsfd=accept(server_fd,(struct sockaddr*)&address,&addrlen);
                  int c=0;
                  c=fork();
                  if(c==0)
                  {
                         close(server_fd);
                         Telecast(nsfd);
                         close(nsfd);
                         return 0; 
                  }
                  close(nsfd);
          }
    }
    return 0;
}

