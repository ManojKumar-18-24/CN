#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

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
// Sender process with IPHDRINCL = 1.

int main() {
    int sockfd;
    struct sockaddr_in dest;
    char packet[60];  // Reduce packet size (MTU-friendly)

    // Create a raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set IP_HDRINCL to 1 to include the IP header manually
    int one = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("Cannot set IP_HDRINCL");
        return 1;
    }

    // Destination address
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, "172.24.254.113", &dest.sin_addr);  // Change to a valid IP

    // Construct the IP header
    struct ip *ip_hdr = (struct ip *)packet;
    ip_hdr->ip_hl = 5;               // Header length (20 bytes)
    ip_hdr->ip_v = 4;                // IPv4
    ip_hdr->ip_tos = 0;              // Type of Service
    ip_hdr->ip_len = htons(40);      // Total length (header + data: 40 bytes)
    ip_hdr->ip_id = htons(54321);    // Identification
    ip_hdr->ip_off = 0;              // Fragment offset
    ip_hdr->ip_ttl = 64;             // Time to Live
    ip_hdr->ip_p = IPPROTO_ICMP;     // Protocol (ICMP)
    inet_pton(AF_INET, "192.168.1.2", &ip_hdr->ip_src); // Source IP
    inet_pton(AF_INET, "172.24.254.113", &ip_hdr->ip_dst); // Destination IP
    ip_hdr->ip_sum = checksum(ip_hdr, sizeof(struct ip));  // Calculate checksum

    // Send the packet (IP header + data)
    if (sendto(sockfd, packet, 40, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Send failed");
    } else {
        printf("Packet sent!\n");
    }

    close(sockfd);
    return 0;
}
