// Server side C program to demonstrate Socket
// programming
#include <netinet/in.h>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;
#define PORT 8080
FILE * pipe_p1=NULL;
FILE * pipe_p2=NULL;
FILE * pipe_p3=NULL;

void Chat(int &nsfd)
{
       char buffer[1024],buf[256];
       recv(nsfd,buffer,1023,0);
       int n=buffer[0]-'0';
       cout<<n<<endl;
       switch(n)
       {
           case 1:  cout<<"before\n";
                         if(pipe_p1==NULL)pipe_p1=popen("./s2","r");
                          //char buf[256];
                          cout<<"beforeno\n";
                          if(pipe_p1==NULL)cout<<"hey\n";
                          dup2(nsfd,fileno(pipe_p1));
                          /*for(int i=0;i<1;i++)
                          {      cout<<"in for loop";
                                  if(fgets(buf,sizeof(buf),pipe_p1))
                                  {
                                       send(nsfd,buf,strlen(buf),0);
                                  }
                          }*/
                          break;
            case 2:  cout<<"before\n";
                          if(pipe_p2==NULL)pipe_p2=popen("./s3","r");
                          //char buf[256];
                          cout<<"beforeno\n";
                          if(pipe_p2==NULL)cout<<"hey\n";
                          dup2(nsfd,fileno(pipe_p2));
                          /*for(int i=0;i<1;i++)
                          {          cout<<"in for loop";
                                  if(fgets(buf,sizeof(buf),pipe_p2))
                                  {
                                       send(nsfd,buf,strlen(buf),0);
                                  }
                          }*/
                          break;
           case 3:   cout<<"before\n";
                          if(pipe_p3==NULL)pipe_p3=popen("./s4","r");
                          //char buf[256];
                          cout<<"beforeno\n";
                          if(pipe_p3==NULL)cout<<"hey\n";
                          dup2(nsfd,fileno(pipe_p3));
                          /*for(int i=0;i<1;i++)
                          {
                                  cout<<"in for loop";
                                  if(fgets(buf,sizeof(buf),pipe_p3))
                                  {
                                       send(nsfd,buf,strlen(buf),0);
                                  }
                          }*/
                          break;      
       }
       close(nsfd);
       return;
}

int main(int argc, char const* argv[])
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char hello[] = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
  while(1)    
 {   
      if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  &addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
      int c=0;
      c=fork();
      if(c==0)
       {     
              close(server_fd);
              Chat(new_socket);
              break;
       }
       close(new_socket);
       sleep(2);
  }
    // closing the listening socket
    close(server_fd);
    return 0;
}

