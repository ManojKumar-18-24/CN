#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ip.h>

#define BUFFER_SIZE 65536

// Change this to your machine's IP address
#define LOCAL_IP "127.0.0.1"

// Function to check if the packet is outgoing
int is_outgoing(struct iphdr *iph) {
    return strcmp(inet_ntoa(*(struct in_addr *)&iph->saddr), LOCAL_IP) == 0;
}

void capture_outgoing_packets() {
    int sock_raw;
    unsigned char buffer[BUFFER_SIZE];
    struct sockaddr saddr;
    socklen_t saddr_len = sizeof(saddr);

    // Create a raw socket bound to the network interface
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening for outgoing packets on the interface...\n");

    while (1) {
        // Receive packets
        int packet_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, &saddr, &saddr_len);
        if (packet_size < 0) {
            perror("recvfrom failed");
            close(sock_raw);
            exit(EXIT_FAILURE);
        }

        // Extract IP header
        struct iphdr *iph = (struct iphdr *)(buffer + 14); // Skip Ethernet header (14 bytes)

        // Check if it's an outgoing packet
        if (is_outgoing(iph)) {
            printf("Outgoing Packet:\n");
            printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
            printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
            printf("Protocol: %u\n", iph->protocol);

            // Extract and print data
            unsigned char *data = buffer + 14 + (iph->ihl * 4);
            int data_len = packet_size - (14 + (iph->ihl * 4));
            if (data_len > 0) {
                printf("Data: %.*s\n\n", data_len, data);
            } else {
                printf("No data in packet\n");
            }
        }
    }

    close(sock_raw);
}

int main() {
    capture_outgoing_packets();
    return 0;
}
