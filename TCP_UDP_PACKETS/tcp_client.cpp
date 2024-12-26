#include <bits/stdc++.h>
#include <netinet/tcp.h>  // Provides declarations for tcp header
#include <netinet/ip.h>   // Provides declarations for ip header
#include <sys/socket.h>
#include <arpa/inet.h>    // Provides inet_pton for address conversion
#include <unistd.h>
#include <string.h>
using namespace std;
// Structure for a TCP header
struct custom_tcphdr {
    uint16_t source_port;  // Source port
    uint16_t dest_port;    // Destination port
    uint32_t seq_num;      // Sequence number
    uint32_t ack_num;      // Acknowledgement number (if ACK flag is set)
    uint8_t data_offset;   // Data offset (size of TCP header in 32-bit words)
    uint8_t flags;         // Control flags (e.g., SYN, ACK, FIN)
    uint16_t window;       // Window size
    uint16_t checksum;     // Checksum (calculated later)
    uint16_t urg_ptr;      // Urgent pointer (if URG flag is set)
};

// Pseudo-header for checksum calculation
struct pseudo_header {
    uint32_t source_ip;
    uint32_t dest_ip;
    uint8_t reserved;  // Reserved, always 0
    uint8_t protocol;  // Protocol (TCP = 6)
    uint16_t tcp_length;  // Length of TCP header + data
};

// Function to calculate checksum
uint16_t checksum(void *data, int length) {
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t *)data;

    while (length > 1) {
        sum += *ptr++;
        length -= 2;
    }

    if (length == 1) {
        sum += *((uint8_t *)ptr);
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (uint16_t)(~sum);
}

int main() {
    // Create a raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    // Destination information
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(80);  // HTTP port
    inet_pton(AF_INET, "192.168.1.1", &dest_addr.sin_addr);  // Replace with actual destination IP

    // Custom TCP header
    struct custom_tcphdr tcp_hdr;
    tcp_hdr.source_port = htons(12345);  // Arbitrary source port
    tcp_hdr.dest_port = htons(80);       // HTTP port
    tcp_hdr.seq_num = htonl(0);          // Initial sequence number
    tcp_hdr.ack_num = 0;                 // No acknowledgment
    tcp_hdr.data_offset = 5 << 4;        // Header size (5 * 32 bits = 20 bytes)
    tcp_hdr.flags = 0x02;                // SYN flag set
    tcp_hdr.window = htons(1024);        // Arbitrary window size
    tcp_hdr.checksum = 0;                // Set to 0 for checksum calculation
    tcp_hdr.urg_ptr = 0;                 // No urgent data

    // Pseudo-header for checksum calculation
    struct pseudo_header pseudo_hdr;
    pseudo_hdr.source_ip = inet_addr("192.168.1.100");  // Replace with actual source IP
    pseudo_hdr.dest_ip = inet_addr("192.168.1.1");      // Replace with actual destination IP
    pseudo_hdr.reserved = 0;
    pseudo_hdr.protocol = IPPROTO_TCP;
    pseudo_hdr.tcp_length = htons(sizeof(tcp_hdr));

    // Combine pseudo-header and TCP header for checksum calculation
    char buffer[1024];
    memcpy(buffer, &pseudo_hdr, sizeof(pseudo_hdr));
    memcpy(buffer + sizeof(pseudo_hdr), &tcp_hdr, sizeof(tcp_hdr));
    tcp_hdr.checksum = checksum(buffer, sizeof(pseudo_hdr) + sizeof(tcp_hdr));

    // Send the TCP packet
    if (sendto(sock, &tcp_hdr, sizeof(tcp_hdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto");
        close(sock);
        return -1;
    }

    cout << "TCP packet sent successfully" << endl;

    close(sock);
    return 0;
}

