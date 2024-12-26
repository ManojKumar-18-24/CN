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
#define PROTOCOL_NUM 75
#define DEST_IP "10.0.2.15"  // Destination IP address (localhost)
#define PORT 12000
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

void send_service_info_client(int &rsfd,int proto_num,string dest_ip)
{
    char packet[4096];
    struct ip *ip_header = (struct ip *)packet;
    struct sockaddr_in dest_addr;
    // Zero out the packet buffer
    memset(packet, 0, sizeof(packet));
    // Fill in the IP header fields
    ip_header->ip_hl = 5;                   // Header length
    ip_header->ip_v = 4;                    // IPv4
    ip_header->ip_tos = 0;                  // Type of Service
    ip_header->ip_len = sizeof(struct ip) + sizeof("12000 I will captal your string\n"); // Total length
    ip_header->ip_id = htonl(54321);        // Identification
    ip_header->ip_off = 0;                  // Fragment offset
    ip_header->ip_ttl = 255;                // Time to Live
    ip_header->ip_p = proto_num;         // Custom protocol number
    ip_header->ip_sum = 0;                  // Initial checksum
    ip_header->ip_src.s_addr = inet_addr("10.0.2.15");  // Source IP
    ip_header->ip_dst.s_addr = inet_addr(DEST_IP);      // Destination IP

    // Calculate IP checksum
    ip_header->ip_sum = checksum(packet, ip_header->ip_len);

    // Add data after the IP header
    char *data = packet + sizeof(struct ip);
    strcpy(data, "12000 I will captal your string\n");

    // Prepare destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

    // Send the packet
    if (sendto(rsfd, packet, ip_header->ip_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Packet sent to client.\n");
}

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
	return sfd;
}

void serving(int &nsfd)
{
        char buffer[1024];
        int read=recv(nsfd,buffer,1024,0);
        for(int i=0;i<read;i++)buffer[i]='a';
        buffer[read]='\n';
        send(nsfd,buffer,read+1,0);
}

void * serve(void* args)
{
        int sfd=co_server(PORT);
        struct sockaddr_in cli_addr;
        socklen_t cli_len;
        struct pollfd fds[1];
        fds[0].fd=sfd;
        fds[0].events=POLLIN;
        cli_len=sizeof(cli_addr);
        while(1)
        {
                poll(fds,1,2000);
                if(fds[0].revents & POLLIN)
                {
                        int nsfd=accept(sfd , (struct sockaddr *)&cli_addr , &cli_len);
                        serving(nsfd);
                       // send(nsfd,"SERVING YOU\n",12,0);
                        close(nsfd);
                }
        }
}

int main()
{
    pthread_t th;
    pthread_create(&th,NULL,&serve,NULL);
    int sockfd;
    int rsfd=socket(AF_INET, SOCK_RAW, IPPROTO_RAW);   //to send service info... 
    char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, PROTOCOL_NUM);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening for custom protocol packets...\n");

    while (1) {
        // Receive the packet
        ssize_t data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
        if (data_size < 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        // Extract IP header
        //struct ip *ip_header = (struct ip *)buffer;
        struct iphdr *ip = (struct iphdr *)buffer;
        string source_ip=inet_ntoa(*(in_addr*)&ip->saddr);
        send_service_info_client(rsfd,76,source_ip);
    }

    close(sockfd);
    return 0;
}
