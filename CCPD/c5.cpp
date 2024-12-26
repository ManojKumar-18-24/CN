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
#define PORT 20004
#define PORT2 15004

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

int get_sock_name(int client_fd)
{
      struct sockaddr_in addr;
      socklen_t addr_len=sizeof(addr);
      if (getsockname(client_fd, (struct sockaddr *)&addr, &addr_len) == -1) {
        perror("getsockname");
        //exit(EXIT_FAILURE);
    }
    // Print out the IP address and port
    //printf("Local IP address: %s\n", inet_ntoa(addr.sin_addr));
    //printf("Local port: %d\n", ntohs(addr.sin_port));
    return  ntohs(addr.sin_port);
}

int main()
{
    int atfd=generate_client(PORT);
    int jcfd=generate_client(PORT2);
        int port,marks;
        bool selected=false;
    for(int j=0;j<2;j++)
    {
          recv(jcfd,&port,sizeof(int),0);
          recv(jcfd,&marks,sizeof(int),0);
          cout<<port<<" "<<marks<<endl;
          if(port==get_sock_name(atfd))selected=true;
    }
    if(selected)cout<<"Hurray!!! Gorre"<<endl;
    return 0;
}
