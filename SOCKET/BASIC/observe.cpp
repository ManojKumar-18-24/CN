// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <string.h>
// // #include <unistd.h>
// // #include <arpa/inet.h>
// // #include <netinet/ip.h>
// // #include <netinet/tcp.h>
// // #include <sys/socket.h>
// // #include<iostream>
// // using namespace std;
// // #define PACKET_SIZE 65536  // Max packet size

// // // Function to check if packet is outgoing
// // int is_outgoing(struct iphdr *iph) {
// //     // Compare with your local IP to determine if it's outgoing
// //     // For simplicity, assuming outgoing from 'source_ip'
// //     const char *source_ip = "127.0.0.1";  // Change this to your local IP
// //     if (strcmp(inet_ntoa(*(struct in_addr *)&iph->saddr), source_ip) == 0) {
// //         return 1;  // Outgoing
// //     }
// //     return 0;  // Not outgoing
// // }

// // int main() {
// //     int sock_raw;
// //     struct sockaddr_in source;
// //     unsigned char *buffer = (unsigned char *)malloc(PACKET_SIZE);
// //     struct iphdr *iph;
// //     struct tcphdr *tcph;

// //     // Create a raw socket to capture TCP packets
// //     sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
// //     if (sock_raw < 0) {
// //         perror("Socket creation failed");
// //         return 1;
// //     }

// //     // Set socket option to include the IP header
// //     const int optval = 1;
// //     if (setsockopt(sock_raw, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0) {
// //         perror("Set socket option failed");
// //         return 1;
// //     }

// //     while (1) {
// //         // Receive packets
// //         int data_size = recv(sock_raw, buffer, PACKET_SIZE, 0);
// //         if (data_size < 0) {
// //             perror("Recv failed");
// //             return 1;
// //         }
// //         cout<<"received\n";
// //         // Point to the IP header
// //         iph = (struct iphdr *)(buffer);
// //         if (iph->protocol == IPPROTO_TCP) {
// //             tcph = (struct tcphdr *)(buffer + iph->ihl * 4);  // TCP header

// //             // Check if the packet is outgoing
// //             if (is_outgoing(iph)) {
// //                 printf("Outgoing TCP Packet:\n");
// //                 printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
// //                 printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
// //                 printf("TCP Src Port: %u\n", ntohs(tcph->source));
// //                 printf("TCP Dst Port: %u\n", ntohs(tcph->dest));
// //                 printf("\n");
// //             }
// //         }
// //     }

// //     close(sock_raw);
// //     return 0;
// // }

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <netinet/ip.h>
// #include <netinet/tcp.h>
// #include <sys/socket.h>

// #define PACKET_SIZE 65536  // Max packet size

// // Function to check if the packet is outgoing
// int is_outgoing(struct iphdr *iph) {
//     // Compare with your local IP to determine if it's outgoing
//     const char *source_ip = "127.0.0.1";  // Change this to your local IP
//     if (strcmp(inet_ntoa(*(struct in_addr *)&iph->saddr), source_ip) == 0) {
//         return 1;  // Outgoing
//     }
//     return 0;  // Not outgoing
// }

// // Function to print the actual data (payload) of the packet
// void print_data(unsigned char *data, int size) {
//     printf("Data:\n");
//     for (int i = 0; i < size; i++) {
//         // Print each byte as a character, but ensure it's a printable ASCII character
//         if (data[i] >= 32 && data[i] <= 126) {
//             printf("%c", data[i]);
//         } else {
//             // If it's not a printable character, print a dot to indicate non-printable characters
//             printf(".");
//         }
//     }
//     printf("\n");
// }

// int main() {
//     int sock_raw;
//     struct sockaddr_in source;
//     unsigned char *buffer = (unsigned char *)malloc(PACKET_SIZE);
//     struct iphdr *iph;
//     struct tcphdr *tcph;

//     // Create a raw socket to capture TCP packets
//     sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
//     if (sock_raw < 0) {
//         perror("Socket creation failed");
//         return 1;
//     }

//     // Set socket option to include the IP header
//     const int optval = 1;
//     if (setsockopt(sock_raw, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0) {
//         perror("Set socket option failed");
//         return 1;
//     }

//     while (1) {
//         // Receive packets
//         int data_size = recv(sock_raw, buffer, PACKET_SIZE, 0);
//         if (data_size < 0) {
//             perror("Recv failed");
//             return 1;
//         }

//         // Point to the IP header
//         iph = (struct iphdr *)(buffer);
//         if (iph->protocol == IPPROTO_TCP) {
//             tcph = (struct tcphdr *)(buffer + iph->ihl * 4);  // TCP header

//             // Check if the packet is outgoing
//             if (is_outgoing(iph)) {
//                 printf("Outgoing TCP Packet:\n");
//                 printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
//                 printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
//                 printf("TCP Src Port: %u\n", ntohs(tcph->source));
//                 printf("TCP Dst Port: %u\n", ntohs(tcph->dest));

//                 // Calculate the start of the data (after IP and TCP headers)
//                 unsigned char *data = buffer + iph->ihl * 4 + tcph->doff * 4;
//                 int data_len = data_size - (iph->ihl * 4 + tcph->doff * 4);  // Remaining data length

//                 // Print the actual data (payload)
//                 print_data(data, data_len);
//                 printf("\n");
//             }
//         }
//     }

//     close(sock_raw);
//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#define PACKET_SIZE 65536  // Max packet size

// Function to check if the packet is outgoing
int is_outgoing(struct iphdr *iph) {
    // Compare with your local IP to determine if it's outgoing
    const char *source_ip = "127.0.0.1";  // Localhost IP address for this case
    if (strcmp(inet_ntoa(*(struct in_addr *)&iph->saddr), source_ip) == 0) {
        return 1;  // Outgoing
    }
    return 0;  // Not outgoing
}

// Function to print the actual data (payload) of the packet
void print_data(unsigned char *data, int size) {
    printf("Data:\n");
    if (size == 0) {
        printf("No data\n");
        return;
    }

    // Print each byte as a character, but ensure it's a printable ASCII character
    for (int i = 0; i < size; i++) {
        // Print each byte as a character, but ensure it's a printable ASCII character
        if (data[i] >= 32 && data[i] <= 126) {
            printf("%c", data[i]);
        } else {
            // If it's not a printable character, print a dot to indicate non-printable characters
            printf(".");
        }
    }
    printf("\n");
}

int main() {
    int sock_raw;
    struct sockaddr_in source;
    unsigned char *buffer = (unsigned char *)malloc(PACKET_SIZE);
    struct iphdr *iph;
    struct tcphdr *tcph;

    // Create a raw socket to capture TCP packets
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set socket option to include the IP header
    const int optval = 1;
    if (setsockopt(sock_raw, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0) {
        perror("Set socket option failed");
        return 1;
    }

    while (1) {
        // Receive packets
        int data_size = recv(sock_raw, buffer, PACKET_SIZE, 0);
        if (data_size < 0) {
            perror("Recv failed");
            return 1;
        }

        // Point to the IP header
        iph = (struct iphdr *)(buffer);
        if (iph->protocol == IPPROTO_TCP) {
            tcph = (struct tcphdr *)(buffer + iph->ihl * 4);  // TCP header

            // Check if the packet is outgoing
            if (is_outgoing(iph)) {
                printf("Outgoing TCP Packet:\n");
                printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
                printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
                printf("TCP Src Port: %u\n", ntohs(tcph->source));
                printf("TCP Dst Port: %u\n", ntohs(tcph->dest));

                // Calculate the start of the data (after IP and TCP headers)
                unsigned char *data = buffer + iph->ihl * 4 + tcph->doff * 4;
                int data_len = data_size - (iph->ihl * 4 + tcph->doff * 4);  // Remaining data length

                // Print the actual data (payload) only if it's present
                if (data_len > 0) {
                    print_data(data, data_len);
                } else {
                    printf("No data in packet\n");
                }
                printf("\n");
            }
        }
    }

    close(sock_raw);
    return 0;
}

