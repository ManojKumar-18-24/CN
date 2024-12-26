#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define DEST_IP "10.0.2.15"  // Destination IP address (localhost)
#define PROTOCOL_NUM 6       // Custom Protocol Number

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

int main() {
    int sockfd;
    char packet[4096];
    struct ip *ip_header = (struct ip *)packet;
    struct tcphdr *tcp_header = (struct tcphdr *)(packet + sizeof(struct ip));
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
    ip_header->ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr) + sizeof("Hello Client")); // Total length
    ip_header->ip_id = htonl(54321);        // Identification
    ip_header->ip_off = 0;                  // Fragment offset
    ip_header->ip_ttl = 255;                // Time to Live
    ip_header->ip_p = PROTOCOL_NUM;         // Custom protocol number
    ip_header->ip_sum = 0;                  // Initial checksum
    ip_header->ip_src.s_addr = inet_addr("172.03.0.1");  // Source IP
    ip_header->ip_dst.s_addr = inet_addr(DEST_IP);      // Destination IP

    // Calculate IP checksum
    ip_header->ip_sum = checksum(packet, sizeof(struct ip));

    // Fill in the TCP header fields
    tcp_header->source = htons(1234);       // Source port
    tcp_header->dest = htons(5678);         // Destination port
    tcp_header->seq = htonl(1);             // Sequence number
    tcp_header->ack_seq = 0;                // Acknowledgment number
    tcp_header->doff = 5;                   // TCP header size
    tcp_header->syn = 1;                    // SYN flag
    tcp_header->check = 0;                  // Checksum (initially 0)
    tcp_header->window = htons(65535);      // Window size
    tcp_header->urg_ptr = 0;                // Urgent pointer

    // Add data after the TCP header
    char *data = packet + sizeof(struct ip) + sizeof(struct tcphdr);
    strcpy(data, "Hello Client");

    // Prepare destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

    // Send the packet
    if (sendto(sockfd, packet, ntohs(ip_header->ip_len), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Packet with TCP header sent to client.\n");

    close(sockfd);
    return 0;
}

