#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

int main() {
    int sockfd;
    char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    // Create a raw socket to capture TCP packets
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    printf("Listening for TCP packets...\n");

    while (1) {
        // Receive the packet
        ssize_t data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
        if (data_size < 0) {
            perror("Receive failed");
            return 1;
        }

        // Extract the IP header
        struct ip *ip_hdr = (struct ip *)buffer;
        int ip_hdr_len = ip_hdr->ip_hl * 4;  // IP header length in bytes

        // Extract the TCP header
        struct tcphdr *tcp_hdr = (struct tcphdr *)(buffer + ip_hdr_len);

        printf("Received TCP packet:\n");
        printf("From: %s\n", inet_ntoa(sender_addr.sin_addr));
        printf("Source Port: %d, Destination Port: %d\n", ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest));
        printf("Sequence Number: %u, Ack Number: %u\n", ntohl(tcp_hdr->seq), ntohl(tcp_hdr->ack_seq));
    }

    close(sockfd);
    return 0;
}
