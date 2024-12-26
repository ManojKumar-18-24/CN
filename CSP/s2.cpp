#include <bits/stdc++.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
using namespace std;
#define PORT 12002

int udp_sfd;

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

void service(string client_ip)
{
    cout<<"ip= "<<client_ip<<endl;
    string DEST_IP="172.20.188.119";
    //client_ip.c_str();
    int sockfd;
    char packet[4096];
    struct ip *ip_header = (struct ip *)packet;
    struct sockaddr_in dest_addr;

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Zero out the packet buffer
    memset(packet, 0, sizeof(packet));

    // Fill in the IP header fields
    ip_header->ip_hl = 5;                   // Header length
    ip_header->ip_v = 4;                    // IPv4
    ip_header->ip_tos = 0;                  // Type of Service
    ip_header->ip_len = sizeof(struct ip) + sizeof("Hello Client you are serviced by s2"); // Total length
    ip_header->ip_id = htonl(54321);        // Identification
    ip_header->ip_off = 0;                  // Fragment offset
    ip_header->ip_ttl = 255;                // Time to Live
    ip_header->ip_p = PROTOCOL_NUM;         // Custom protocol number
    ip_header->ip_sum = 0;                  // Initial checksum
    ip_header->ip_src.s_addr = inet_addr("172.03.255.1");  // Source IP
    ip_header->ip_dst.s_addr = inet_addr(DEST_IP.c_str());      // Destination IP

    // Calculate IP checksum
    ip_header->ip_sum = checksum(packet, ip_header->ip_len);

    // Add data after the IP header
    char *data = packet + sizeof(struct ip);
    strcpy(data, "Hello Client you are serviced by s2");

    // Prepare destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP.c_str());

    // Send the packet
    if (sendto(sockfd, packet, ip_header->ip_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }
}

void get_udp_server()
{
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    int port_no = PORT;
    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    perror("\n socket ");
    else
        printf("\n socket created successfully");
    int opt=1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        perror("\n bind : ");
    else
        printf("\n bind successful ");
    cli_len = sizeof(cli_addr);
    char buffer[256];
    while(1)
    {
        int r=recvfrom(sfd, buffer, 256, 0, (struct sockaddr *)&cli_addr, &cli_len);
        cout<<"buffer= "<<buffer<<endl;
        string ip;
        int i = 0;
        while (buffer[i] != '\n' && i<r)
        {
            ip.push_back(buffer[i]);
            i++;
        }
        service(ip);
    }
}

int main()
{
    get_udp_server();
    return 0;
}