#include<bits/stdc++.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<poll.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

void ChildFunc(int &new_socket)
{
    char buffer[1024];
    char hello[]="mahesh.. machaleni mahesh\n";
    int valread = read(new_socket, buffer,
                   1024 - 1); 
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent from server\n");
}

int main()
{
  int co_socket,cl_socket;
  struct pollfd fds[2];
  nfds_t nfs=2;
  struct sockaddr_in addr1,addr2;sockaddr addr3;
  socklen_t co=sizeof(addr1),cl=sizeof(addr2);
  co_socket=socket(AF_INET,SOCK_STREAM,0);
  cl_socket=socket(AF_INET,SOCK_DGRAM,0);
  int opt=1;
  setsockopt(co_socket,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));
  //setsockopt(cl_socket,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));
  memset(&addr1,0,sizeof(addr1));
  memset(&addr2,0,sizeof(addr1));
  memset(&addr3,0,sizeof(addr1));
  inet_pton(AF_INET, "127.0.0.1", &addr1.sin_addr);
  addr1.sin_family=AF_INET;
  addr1.sin_addr.s_addr=INADDR_ANY;
  addr1.sin_port=htons(9999);
  bind(co_socket,(struct  sockaddr*)&addr1,sizeof(addr1));
  listen(co_socket,3);
  addr2.sin_family = AF_INET; // IPv4
  addr2.sin_addr.s_addr = INADDR_ANY;
  addr2.sin_port = htons(9345);
  bind(cl_socket,(struct sockaddr*)&addr2,sizeof(addr2));
  fds[0].fd=co_socket;
  fds[0].events=POLLIN;
  fds[1].fd=cl_socket;
  fds[1].events=POLLIN;
  socklen_t len=sizeof(addr3);
  while(1)
  {
       poll(fds,2,10000);
       if(fds[0].revents & POLLIN)
       {
            cout<<"connection oriented got polled\n";
            int nsfd=accept(co_socket,(struct sockaddr*)&addr1,&co);
            ChildFunc(nsfd);
            close(nsfd);
       }
       if(fds[1].revents & POLLIN)
       {
             cout<<"connection less got polled\n";
             char buffer[1024];
             int n=recvfrom(cl_socket,(char *) buffer,1024,MSG_WAITALL,(struct sockaddr*)&addr3,&len);
             cout<<buffer<<endl; 
       }
  }
  return 0;
 }
