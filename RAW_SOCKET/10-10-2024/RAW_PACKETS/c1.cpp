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
#define DEST_IP "10.0.2.15"  // Destination IP address (localhost)
#define PROTOCOL_NUM 75       // Custom Protocol Number

// Function to calculate checksum (used in the IP header)
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short*)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
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

void * get_service(void* args)
{
       int port=*(int*)args;
       int clfd=generate_client(port);
       send(clfd,"Serve_me_plijj\n",15,0);
       char msg[1024];
       int read=recv(clfd,msg,1024,0);
       cout<<"\nMessage from server: on port "<<port<<endl;
       for(int i=0;i<read;i++)cout<<msg[i];
       cout<<endl;
       close(clfd);
}


int main() {
    int sockfd;
    char packet[4096];
    char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    struct ip *ip_header = (struct ip *)packet;
    struct sockaddr_in dest_addr;
    int rsfd=socket(AF_INET, SOCK_RAW, 76);
    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    pthread_t th[100];
    int index=0;
    while(1)
    {
              // Zero out the packet buffer
             memset(packet, 0, sizeof(packet));

             // Fill in the IP header fields
             ip_header->ip_hl = 5;                   // Header length
             ip_header->ip_v = 4;                    // IPv4
             ip_header->ip_tos = 0;                  // Type of Service
             ip_header->ip_len = sizeof(struct ip) + sizeof("Hello Client"); // Total length
             ip_header->ip_id = htonl(54321);        // Identification
             ip_header->ip_off = 0;                  // Fragment offset
             ip_header->ip_ttl = 255;                // Time to Live
             ip_header->ip_p = PROTOCOL_NUM;         // Custom protocol number
             ip_header->ip_sum = 0;                  // Initial checksum
             ip_header->ip_src.s_addr = inet_addr("172.03.0.1");  // Source IP
             ip_header->ip_dst.s_addr = inet_addr(DEST_IP);      // Destination IP

             // Calculate IP checksum
             ip_header->ip_sum = checksum(packet, ip_header->ip_len);

             // Add data after the IP header
             char *data = packet + sizeof(struct ip);
             strcpy(data, "Hello Client");

              // Prepare destination address
             dest_addr.sin_family = AF_INET;
             dest_addr.sin_port = 0;
             dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

              // Send the packet
              if (sendto(sockfd, packet, ip_header->ip_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
                         perror("Sendto failed");
                         exit(EXIT_FAILURE);
                }

                printf("Packet sent to client.\n");
                sleep(2); for(int i=0;i<3;i++){
                 ssize_t data_size = recvfrom(rsfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
                 if (data_size < 0) {
                           perror("Receive failed");
                           exit(EXIT_FAILURE);
                 }
                 char *ndata = buffer + ip_header->ip_hl * 4;
                 printf("Data: %s\n", ndata);
                 int port=0;
                 while(*(ndata)!=' ')
                 {
                      port*=10;
                      port+=(*(ndata)-'0');
                      ++ndata;
                 }
                 int *a=(int*)malloc(sizeof(int));
                 *a=port;
                 pthread_create(&th[index],NULL,&get_service,a);
                 index++;}
                 sleep(3);
    }
    close(sockfd);
    return 0;
}
