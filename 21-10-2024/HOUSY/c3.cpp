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
#define PORT 30002

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
     int rsfd=socket(AF_INET, SOCK_RAW, 78);
     int clfd=generate_client(PORT);
     vector<int> coupon(5);
     for(int i=0;i<5;i++)recv(clfd,&coupon[i],sizeof(int),0);
     for(int i=0;i<5;i++)cout<<coupon[i]<<" ";
     cout<<endl;
     char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
     int sent=true;
     for(int i=0;i<5;i++)
     {
                 ssize_t data_size = recvfrom(rsfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
                 if (data_size < 0) {
                           perror("Receive failed");
                           exit(EXIT_FAILURE);
                 }
                  struct ip *ip_header = (struct ip *)buffer;  // IP header is at the start of the buffer
                  char *data = buffer + (ip_header->ip_hl * 4);  // Skip IP header to access data

                 // Print source IP address from IP header
                 printf("Packet received from: %s\n", inet_ntoa(ip_header->ip_src));  // Accessing source IP from IP header

                 // Print the actual data received
                 printf("Data: %s\n", data);
     }
     cout<<"received\n";
     while(1);
     return 0;
} 
