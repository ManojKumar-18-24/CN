#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h> // Provides declarations for tcp header
#include <netinet/ip.h>  // Provides declarations for ip header
#include <unistd.h>

// Pseudo header needed for checksum calculation
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

// Calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1, len -= 2;)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
  while(1){
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Raw socket creation failed");
        return 1;
    }

    char packet[4096];
    memset(packet, 0, 4096); // Clear the packet buffer

    struct ip *ip_hdr = (struct ip *) packet;
    struct tcphdr *tcp_hdr = (struct tcphdr *) (packet + sizeof(struct ip));

    // Create the IP header
    ip_hdr->ip_hl = 5;
    ip_hdr->ip_v = 4;
    ip_hdr->ip_tos = 0;
    ip_hdr->ip_len = sizeof(struct ip) + sizeof(struct tcphdr) + strlen("Hello Client");
    ip_hdr->ip_id = htonl(54321); // ID of this packet
    ip_hdr->ip_off = 0;
    ip_hdr->ip_ttl = 255;
    ip_hdr->ip_p = IPPROTO_TCP;
    ip_hdr->ip_sum = 0; // Kernel will calculate the checksum for us
    ip_hdr->ip_src.s_addr = inet_addr("10.0.2.15"); // Server IP
    ip_hdr->ip_dst.s_addr = inet_addr("10.0.2.15"); // Client IP

    // Create the TCP header
    tcp_hdr->source = htons(12345); // Arbitrary source port
    tcp_hdr->dest = htons(8080); // Destination port (client listening)
    tcp_hdr->seq = 0;
    tcp_hdr->ack_seq = 0;
    tcp_hdr->doff = 5; // TCP header size
    tcp_hdr->fin = 0;
    tcp_hdr->syn = 1;
    tcp_hdr->rst = 0;
    tcp_hdr->psh = 1;
    tcp_hdr->ack = 0;
    tcp_hdr->urg = 0;
    tcp_hdr->window = htons(5840); // Maximum allowed window size
    tcp_hdr->check = 0; // Leave the checksum 0 now, filled later by pseudo-header
    tcp_hdr->urg_ptr = 0;

    // Pseudo-header for checksum calculation
    struct pseudo_header psh;
    psh.source_address = inet_addr("10.0.2.15"); // Server IP
    psh.dest_address = inet_addr("10.0.2.15");   // Client IP
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + strlen("Hello Client"));

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen("Hello Client");
    char *pseudogram = (char*)malloc(psize);

    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcp_hdr, sizeof(struct tcphdr) + strlen("Hello Client"));

    tcp_hdr->check = checksum(pseudogram, psize);

    // Add the data (Hello Client)
    char *data = packet + sizeof(struct ip) + sizeof(struct tcphdr);
    strcpy(data, "Hello Client");

    // Send the packet
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(8080);
    dest.sin_addr.s_addr = inet_addr("10.0.2.15"); // Client's IP

    if (sendto(sockfd, packet, ip_hdr->ip_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Send failed");
    } else {
        printf("Packet sent to client\n");
    }

    free(pseudogram);
    close(sockfd); 
    }
    return 0;
}

