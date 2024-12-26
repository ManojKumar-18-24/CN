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
#define BUF_LEN 1024

int main()
{
        int rsfd=socket(AF_INET,SOCK_RAW,IPPROTO_RAW);
        if(rsfd==-1)
        {
               cout<<"failed\n";
               return 0;
        }
	char buf[BUF_LEN];
	struct sockaddr_in client;
	socklen_t clilen=sizeof(client);
	cout<<"receive"<<endl;
	recvfrom(rsfd,buf,BUF_LEN,0,(sockaddr*)&client,(socklen_t*)clilen);
	perror("recv");
	struct iphdr *ip;
        ip=(struct iphdr*)buf;
	cout<<(buf+(ip->ihl)*4)<<endl;
     return 0;
}
