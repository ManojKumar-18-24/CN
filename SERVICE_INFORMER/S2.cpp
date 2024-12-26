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
#define PORT 12335
#define PORT2 9045

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

	listen(sfd,10);

	cli_len=sizeof(cli_addr);
	return sfd;
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

void Service(int &nsfd)
{
         send(nsfd,"You_got_serviced_from_S1\0",25,0);
}

int main()
{
    int nsfd=co_server();
    sleep(2);
    int clfd=generate_client(PORT2);
    int r=PORT;
    send(clfd,&r,sizeof(r),0);
    cout<<"port sent\n";
    close(clfd);
    struct pollfd fds[1];
    fds[0].fd=nsfd;
    fds[0].events=POLLIN;
    while(1)
    {
        poll(fds,1,2000);
        if(fds[0].revents & POLLIN)
        {
              struct sockaddr_in cli_addr;
              socklen_t cli_len=sizeof(cli_addr);
              int nsfd=accept(fds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);
              cout<<"\nservicing\n";
              Service(nsfd);
              close(nsfd);
        }
         sleep(1);
    }
    return 0;
}
