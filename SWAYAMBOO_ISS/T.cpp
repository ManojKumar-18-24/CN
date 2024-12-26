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
#define PORT 8096

struct message{
      long msg_type;
      int port;
};

int msgid;
int co_server()
{
        int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
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

int main()
{
   vector<int> ports;
   int csfd=co_server();
   struct pollfd fds[1];
   fds[0].fd=csfd;
   fds[0].events=POLLIN;
   key_t key;
   key=ftok("queue",65);
   msgid=msgget(key,0666|IPC_CREAT);
   message msg;
   while(1)
   {
         poll(fds,1,2000);
         if(fds[0].revents & POLLIN)
         {
              struct sockaddr_in cli_addr;
              socklen_t cli_len=sizeof(cli_addr);
              int nsfd=accept(fds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);
              send_vector(nsfd,ports);
              cout<<"\nvector_sent\n";
              close(nsfd);
         }
         int ret=msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),1,IPC_NOWAIT);
         if(ret!=-1)
         {
               cout<<msg.port<<" received\n";
               ports.push_back(msg.port);
               msg.msg_type=2;
               msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
         }
         sleep(1);
   }
   return 0;
}
