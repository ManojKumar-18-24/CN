#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

using namespace std;

// TCP header structure (manually defined for clarity)
struct custom_tcphdr {
    uint16_t source_port;  // Source port
    uint16_t dest_port;    // Destination port
    uint32_t seq_num;      // Sequence number
    uint32_t ack_num;      // Acknowledgement number
    uint8_t data_offset;   // Data offset (header size)
    uint8_t flags;         // Control flags (SYN, ACK, etc.)
    uint16_t window;       // Window size
    uint16_t checksum;     // Checksum
    uint16_t urg_ptr;      // Urgent pointer
};

// Function to print the details of the TCP header
void print_tcp_header(struct tcphdr *tcp_header) {
    cout << "----- TCP Header -----" << endl;
    cout << "Source Port: " << ntohs(tcp_header->source) << endl;
    cout << "Destination Port: " << ntohs(tcp_header->dest) << endl;
    cout << "Sequence Number: " << ntohl(tcp_header->seq) << endl;
    cout << "Acknowledgment Number: " << ntohl(tcp_header->ack_seq) << endl;
    cout << "Flags: " << (unsigned)tcp_header->th_flags << endl;
    cout << "Window Size: " << ntohs(tcp_header->window) << endl;
    cout << "Checksum: " << ntohs(tcp_header->check) << endl;
    cout << "Urgent Pointer: " << ntohs(tcp_header->urg_ptr) << endl;
    cout << "----------------------" << endl;
}

int main() {
    // Create a raw socket to listen for incoming TCP packets
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    // Address of the interface to listen on (any local address)
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0"); // Listen on all interfaces

    cout << "Server is listening for TCP packets..." << endl;

    // Receive buffer
    char buffer[65536];  // Large enough to store entire packet (IP header + TCP header + data)
    
    // Infinite loop to listen for incoming packets
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Receive a packet
        int packet_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
        if (packet_len < 0) {
            perror("recvfrom");
            continue;
        }

        // Extract the IP header
        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct tcphdr *tcp_header = (struct tcphdr *)(buffer + (ip_header->ihl * 4));  // IP header size in 32-bit words

        // Print the received TCP header
        print_tcp_header(tcp_header);

        // Optionally, you can print the data after the TCP header as well
        char *data = (char *)(buffer + (ip_header->ihl * 4) + (tcp_header->doff * 4));  // Skip IP and TCP headers
        int data_len = packet_len - (ip_header->ihl * 4) - (tcp_header->doff * 4);
        if (data_len > 0) {
            cout << "Data: " << string(data, data_len) << endl;
        } else {
            cout << "No data in the packet." << endl;
        }

        cout << "----------------------" << endl;
    }

    close(sock);
    return 0;
}

