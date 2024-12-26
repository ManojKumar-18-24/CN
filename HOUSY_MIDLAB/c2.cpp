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
#define PORT 30001

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
     int clfd=generate_client(PORT);
     vector<int> coupon(3);
     for(int i=0;i<3;i++)recv(clfd,&coupon[i],sizeof(int),0);
     for(int i=0;i<3;i++)cout<<coupon[i]<<" ";
     cout<<endl;
     int sent=true;
     for(int i=0;i<15;i++)
     {
          int r;
          recv(clfd,&r,sizeof(int),0);
          cout<<r<<" ";
          if(sent)
          {
                for(int j=0;j<3 && sent;j++)
                {
                      if(r==coupon[i]){
                            sent=false;
                            r=25;
                            send(clfd,&r,sizeof(int),0);
                            cout<<"sent";
                      }
                }
          }
     }
     cout<<"received\n";
       if(sent==false)
     {
           char buf[6];
          recv(clfd,buf,6,0);
          cout<<buf<<endl;
     }
     sleep(50);
     //while(1);
     return 0;
} 
