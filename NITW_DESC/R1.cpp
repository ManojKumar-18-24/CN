
/*
    NITW DESK
    22CSB0C23
    Samudrala Manojkumar
    R1
*/
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
#include <semaphore.h>
using namespace std;

int srsfd,rrsfd;

#define DEST_IP "172.20.247.122"  // Destination IP address (localhost)
#define PROTOCOL_NUM 76       // Custom Protocol Number

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


void send_string(int rsfd, string message, const char *destination_ip)
{
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(destination_ip);
    // Send the message
    if (sendto(rsfd, message.c_str(), message.length() + 1, 0, (sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto failed");
    }
    else
    {
        std::cout << "Message sent successfully" << std::endl;
    }
}

string receive_string(int rsfd, char *buffer, size_t buffer_size)
{
    sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);
    // Receive the message
    ssize_t received_bytes = recvfrom(rsfd, buffer, buffer_size, 0,
                                      (sockaddr *)&src_addr, &addr_len);
    if (received_bytes < 0)
    {
        perror("recvfrom failed");
    }
    else if (received_bytes > 0)
    {
        buffer[received_bytes] = '\0'; // Null-terminate the received data
        // Skip IP header if using raw sockets (for IPv4)
        char *payload = buffer + 20;
        std::cout << "Received message: " << payload << std::endl;
        std::cout << "From IP: " << inet_ntoa(src_addr.sin_addr) << std::endl;
        return payload;
    }
    else
    {
        std::cout << "No data received." << std::endl;
    }
    return "";
}

void send()
{
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
    memset(packet, 0, sizeof(packet));

    // Fill in the IP header fields
    ip_header->ip_hl = 5;                   // Header length
    ip_header->ip_v = 4;                    // IPv4
    ip_header->ip_tos = 0;                  // Type of Service
    ip_header->ip_len = sizeof(struct ip) + sizeof("Answer from R1"); // Total length
    ip_header->ip_id = htonl(54321);        // Identification
    ip_header->ip_off = 0;                  // Fragment offset
    ip_header->ip_ttl = 255;                // Time to Live
    ip_header->ip_p = IPPROTO_RAW;         // Custom protocol number
    ip_header->ip_sum = 0;                  // Initial checksum
    ip_header->ip_src.s_addr = inet_addr("172.03.0.1");  // Source IP
    ip_header->ip_dst.s_addr = inet_addr(DEST_IP);      // Destination IP

    // Calculate IP checksum
    ip_header->ip_sum = checksum(packet, ip_header->ip_len);

    // Add data after the IP header
    char *data = packet + sizeof(struct ip);
    strcpy(data, "Answer from R1");

    // Prepare destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

    // Send the packet
    if (sendto(sockfd, packet, ip_header->ip_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Packet sent to client.\n");
}

void * recv_question(void *args)
{
    char buffer[1024];
    while(true)
    {
        sleep(5);
        receive_string(rrsfd,buffer,sizeof(buffer));
        send();
    }
}

int main()
{
    srsfd = socket(AF_INET, SOCK_RAW, 75);
    rrsfd=socket(AF_INET,SOCK_RAW,77);
    pthread_t th[2];
    //pthread_create(&th[0],NULL,send_question,NULL);
    pthread_create(&th[1],NULL,recv_question,NULL);
    while(1);
    return 0;
}