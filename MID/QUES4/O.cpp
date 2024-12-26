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
#define PORT 15000
#define PORT2 17000
#define PORT3 18000

vector<int> pcfds;

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

void Accept(vector<int>& sfd,vector<int>&nsfd)
{
    struct pollfd fds[3];
    for(int i=0;i<3;i++)
    {
        fds[i].fd=sfd[i];
        fds[i].events=POLLIN;
    }
    vector<int> mask(3,true);
    int count=0;
    while(count<3)
    {
              poll(fds,3,2000);
              for(int i=0;i<3;i++)
              {
                        if((fds[i].revents & POLLIN) && mask[i])
                        {
                                  cout<<i<<" got polled\n";
                                  struct sockaddr_in addr;
                                  socklen_t len=sizeof(addr);
                                  nsfd[i]=accept(sfd[i],(struct sockaddr*)&addr,&len);
                                  mask[i]=false;
                                  count++;
                        }
              }
    }
    
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

void Do(vector<int>&nsfd,int chatting,int teaching,int j)
{
    struct pollfd fds[3];
    for(int i=0;i<3;i++)
    {
        fds[i].fd=nsfd[i];
        fds[i].events=POLLIN;
    }
    struct pollfd chat[1];
    chat[0].fd=chatting;
    chat[0].revents=POLLIN;
    poll(fds,3,2000);
    for(int i=0;i<3;i++)
    {
          if(fds[i].revents & POLLIN)
          {
                  cout<<"nsfd "<<i<<" got polled\n";
                  int r;
                  recv(nsfd[i],&r,sizeof(int),0);        
                  cout<<r<<endl;
                  send(pcfds[j],&r,sizeof(int),0); 
          }
    }
    char buffer[17];
    recv(teaching,buffer,17,0);
    cout<<"\nfRom teacher :"<<buffer<<endl;
    buffer[16]='\0';
    for(int i=0;i<3;i++)send(nsfd[i],buffer,strlen(buffer),0);
}

/*void Do(vector<int>&nsfd,int chatting,int teaching)
{
    struct pollfd fds[3];
    for(int i=0;i<3;i++)
    {
        fds[i].fd=nsfd[i];
        fds[i].events=POLLIN;
    }
    struct pollfd chat[1];
    chat[0].fd=chatting;
    chat[0].revents=POLLIN;
    poll(fds,3,2000);
    for(int i=0;i<3;i++)
    {
          if(fds[i].revents & POLLIN)
          {
                  cout<<"nsfd "<<i<<" got polled\n";
                  int r;
                  recv(nsfd[i],&r,sizeof(int),0);        
                  cout<<r<<endl;
                  send(chatting,&r,sizeof(int),0); 
          }
    }

    char buffer[100];  // increase buffer size to avoid issues with larger messages
    int bytes_received = recv(teaching, buffer, sizeof(buffer), 0);
    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0';  // Null-terminate the received string
        cout<<"\nFrom teacher: "<<buffer<<endl;
        for(int i=0;i<3;i++)send(nsfd[i],buffer,bytes_received,0);
    }
}*/

int main()
{
    vector<int> sfd(3);
    for(int i=0;i<3;i++)sfd[i]=co_server(PORT+i);
    vector<int> nsfd(3);
    cout<<endl;
    Accept(sfd,nsfd);
    vector<int> pcfd(2);
    for(int i=0;i<2;i++)
    {
         pcfd[i]=generate_client(PORT2+i);
    }
    sleep(3);
    pcfds.resize(2);
    for(int i=0;i<2;i++)
    {
         pcfds[i]=generate_client(PORT3+i);
    }
    for(int i=0;i<2;i++)
    {
             int r=0;
             send(pcfd[0],&r,sizeof(int),0);
             r=1;
             send(pcfd[1],&r,sizeof(int),0);                 /*first cycle*/
             sleep(2);
             Do(nsfd,pcfd[0],pcfd[1],0);
             recv(pcfd[0],&r,sizeof(int),0);
             recv(pcfd[1],&r,sizeof(int),0);
             sleep(2);
             cout<<"First cycle completed\n";
             r=1;
             send(pcfd[0],&r,sizeof(int),0);
             r=0;
             send(pcfd[1],&r,sizeof(int),0);  
             sleep(2);                                                  /*second cycle*/
             Do(nsfd,pcfd[1],pcfd[0],1);
             recv(pcfd[1],&r,sizeof(int),0);  
             recv(pcfd[0],&r,sizeof(int),0);    
             //swap(pcfd[0],pcfd[1]);     
             sleep(2);
            cout<<"second cycle completed\n";
    }
    for(int i=0;i<3;i++)
    {
          send(nsfd[i],"Present to class\0",16,0);
          send(nsfd[i],"Appreciate to you\0",17,0);
    }
    sleep(20);
    return 0;
}
