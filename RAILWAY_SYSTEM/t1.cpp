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
#include<semaphore.h>
using namespace std;
#define PORT 15433

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
	else printf("\nconnect succesful\n");
	return sfd;
}

int main()
{
   srand(time(0));
   vector<string> bhogi={"E0","G1","G2","G3","A","B","C","S1","S2","S3","S4","S5","S6","S7","S8","S9","G1","G2","G3","E1"};
   while(1)
   {
          int r=rand()%3;
          cout<<"r value: "<<r<<endl;
          int clfd=generate_client(PORT+r);
          sleep(2);
          for(int i=0;i<20;i++)
          {
                 send(clfd,bhogi[i].c_str(),2,0);
          }
          send(clfd,"leaving\n",8,0);
          close(clfd);
          sleep(5);
   }
   return 0;
}
