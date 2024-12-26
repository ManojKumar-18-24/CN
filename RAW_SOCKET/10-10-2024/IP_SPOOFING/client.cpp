#include<bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
using namespace std;
#define PROTOCOL_NUM 75  // Custom Protocol Number

void print_ipheader(struct iphdr* ip)
{
	cout<<"------------------------\n";
	cout<<"Printing IP header....\n";
	cout<<"IP version:"<<(unsigned int)ip->version<<endl;
	cout<<"IP header length:"<<(unsigned int)ip->ihl<<endl;
	
	cout<<"Type of service:"<<(unsigned int)ip->tos<<endl;
	cout<<"Total ip packet length:"<<ntohs(ip->tot_len)<<endl;
	cout<<"Packet id:"<<ntohs(ip->id)<<endl;
	cout<<"Time to leave :"<<(unsigned int)ip->ttl<<endl;
	cout<<"Protocol:"<<(unsigned int)ip->protocol<<endl;
	cout<<"Check:"<<ip->check<<endl;
	cout<<"Source ip:"<<inet_ntoa(*(in_addr*)&ip->saddr)<<endl;
	//printf("%pI4\n",&ip->saddr );
	cout<<"Destination ip:"<<inet_ntoa(*(in_addr*)&ip->daddr)<<endl;
	cout<<"End of IP header\n";
	cout<<"------------------------\n";
}

int main() {
    int sockfd;
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
        struct iphdr *ip_header = (struct iphdr *)buffer;
        //char *data = buffer + ip_header->ip_hl * 4;  // IP header length in 32-bit words
        print_ipheader(ip_header);
        //printf("Received packet from: %s\n", inet_ntoa(sender_addr.sin_addr));
        //printf("Data: %s\n", data);
    }

    close(sockfd);
    return 0;
}

