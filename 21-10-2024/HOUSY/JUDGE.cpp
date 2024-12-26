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
#define ADDRESS "judge"

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
    string s;
    for(int i=0;i<5;i++)
    {
           s+=(to_string(rand()%100));
           if(i==4)s+="\n";
           else s+=" ";
    }
    ip_header->ip_len = sizeof(struct ip) + sizeof(s); // Total length
    ip_header->ip_id = htonl(54321);        // Identification
    ip_header->ip_off = 0;                  // Fragment offset
    ip_header->ip_ttl = 255;                // Time to Live
    ip_header->ip_p = proto_num;         // Custom protocol number
    ip_header->ip_sum = 0;                  // Initial checksum
    ip_header->ip_src.s_addr = inet_addr("10.0.2.15");  // Source IP
    ip_header->ip_dst.s_addr = inet_addr(dest_ip.c_str());      // Destination IP

    // Calculate IP checksum
    ip_header->ip_sum = checksum(packet, ip_header->ip_len);

    // Add data after the IP header
    char *data = packet + sizeof(struct ip);
    strcpy(data, s.c_str());

    // Prepare destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip.c_str());

    // Send the packet
    if (sendto(rsfd, packet, ip_header->ip_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Packet sent to client.\n");
}

int main()
{
       sleep(5);
       int rsfd=socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
       for(int i=0;i<5;i++)
       {
              send_service_info_client(rsfd,78,"10.0.2.15");
              cout<<"sent\n";
       }
       while(1);
       return 0;
}
