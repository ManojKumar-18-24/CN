#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
using namespace std;

#define PROTOCOL_NUM 75  // Custom Protocol Number

// Function to print the IP header
void print_ipheader(struct iphdr* ip) {
    printf("------------------------\n");
    printf("Printing IP header....\n");
    printf("IP version: %u\n", (unsigned int)ip->version);
    printf("IP header length: %u\n", (unsigned int)ip->ihl);
    printf("Type of service: %u\n", (unsigned int)ip->tos);
    printf("Total IP packet length: %u\n", ntohs(ip->tot_len));
    printf("Packet ID: %u\n", ntohs(ip->id));
    printf("Time to live: %u\n", (unsigned int)ip->ttl);
    printf("Protocol: %u\n", (unsigned int)ip->protocol);
    printf("Checksum: %u\n", ntohs(ip->check));
    printf("Source IP: %s\n", inet_ntoa(*(struct in_addr*)&ip->saddr));
    printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr*)&ip->daddr));
    printf("End of IP header\n");
    printf("------------------------\n");
}

// Function to print the TCP header
void print_tcpheader(struct tcphdr* tcp) {
    printf("------------------------\n");
    printf("Printing TCP header....\n");
    printf("Source port: %u\n", ntohs(tcp->source));
    printf("Destination port: %u\n", ntohs(tcp->dest));
    printf("Sequence number: %u\n", ntohl(tcp->seq));
    printf("Acknowledgment number: %u\n", ntohl(tcp->ack_seq));
    printf("Header length: %u\n", (unsigned int)tcp->doff * 4);
    printf("SYN flag: %u\n", (unsigned int)tcp->syn);
    printf("Checksum: %u\n", ntohs(tcp->check));
    printf("Window size: %u\n", ntohs(tcp->window));
    printf("End of TCP header\n");
    printf("------------------------\n");
}

int main() {
    int sockfd;
    char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW,IPPROTO_TCP);
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
        struct iphdr *ip_header = (struct iphdr *)buffer;
        print_ipheader(ip_header);

        // Extract TCP header (after IP header)
        struct tcphdr *tcp_header = (struct tcphdr *)(buffer + (ip_header->ihl * 4));
        print_tcpheader(tcp_header);

        // Extract data (after TCP header)
        char *data = (char *)(buffer + (ip_header->ihl * 4) + (tcp_header->doff * 4));
        printf("Data: %s\n", data);
    }

    close(sockfd);
    return 0;
}

