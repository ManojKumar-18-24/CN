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
#include <netinet/tcp.h>
using namespace std;

#define PACKET_SIZE 65536 // Max packet size
#define LOCAL_IP "127.0.0.1"
#define BUFFER_SIZE 65536
map<char, int> company_port;

// Function to check if the packet is outgoing
int is_outgoing(struct iphdr *iph)
{
    // Compare with your local IP to determine if it's outgoing
    const char *source_ip = "127.0.0.1"; // Localhost IP address for this case
    if (strcmp(inet_ntoa(*(struct in_addr *)&iph->saddr), source_ip) == 0)
    {
        return 1; // Outgoing
    }
    return 0; // Not outgoing
}

void *tcp_capture(void *args)
{
    int sock_raw;
    struct sockaddr_in source;
    unsigned char *buffer = (unsigned char *)malloc(PACKET_SIZE);
    struct iphdr *iph;
    struct tcphdr *tcph;

    // Create a raw socket to capture TCP packets
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0)
    {
        perror("Socket creation failed");
        return (void *)1;
    }

    // Set socket option to include the IP header
    const int optval = 1;
    if (setsockopt(sock_raw, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0)
    {
        perror("Set socket option failed");
        return (void *)1;
    }

    while (1)
    {
        // Receive packets
        int data_size = recv(sock_raw, buffer, PACKET_SIZE, 0);
        if (data_size < 0)
        {
            perror("Recv failed");
            return (void *)1;
        }

        // Point to the IP header
        iph = (struct iphdr *)(buffer);
        if (iph->protocol == IPPROTO_TCP)
        {
            tcph = (struct tcphdr *)(buffer + iph->ihl * 4); // TCP header

            // Check if the packet is outgoing
            if (is_outgoing(iph))
            {
                // printf("Outgoing TCP Packet:\n");
                // printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
                // printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
                /// printf("TCP Src Port: %u\n", ntohs(tcph->source));
                // printf("TCP Dst Port: %u\n", ntohs(tcph->dest));

                // Calculate the start of the data (after IP and TCP headers)
                unsigned char *data = buffer + iph->ihl * 4 + tcph->doff * 4;
                int data_len = data_size - (iph->ihl * 4 + tcph->doff * 4); // Remaining data length

                // Print the actual data (payload) only if it's present
                if (data_len > 0)
                {
                    company_port[data[0]] = ntohs(tcph->source);
                    // print_data(data, data_len);
                }
                else
                {
                    // printf("No data in packet\n");
                }
                // printf("\n");
            }
        }
    }
    close(sock_raw);
}

int co_server(int port)
{
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("\n socket ");
    else
        printf("\n socket created successfully");
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        perror("\n bind : ");
    else
        printf("\n bind successful ");
    listen(sfd, 10);
    int nsfd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len);
    if(nsfd==-1)cout<<"\nnsfd=-1\n";
    close(sfd);
    return nsfd;
}

void *capture_outgoing_packets(void *args)
{
    int sock_raw;
    unsigned char buffer[BUFFER_SIZE];
    struct sockaddr saddr;
    socklen_t saddr_len = sizeof(saddr);

    // Create a raw socket bound to the network interface
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening for outgoing packets on the interface...\n");

    while (1)
    {
        // Receive packets
        int packet_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, &saddr, &saddr_len);
        if (packet_size < 0)
        {
            perror("recvfrom failed");
            close(sock_raw);
            exit(EXIT_FAILURE);
        }

        // Extract IP header
        struct iphdr *iph = (struct iphdr *)(buffer + 14); // Skip Ethernet header (14 bytes)

        // Check if it's an outgoing packet
        if (is_outgoing(iph))
        {
            // printf("Outgoing Packet:\n");
            // printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
            // printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
            printf("Protocol: %u\n", iph->protocol);

            // Extract and print data
            unsigned char *data = buffer + 14 + (iph->ihl * 4);
            int data_len = packet_size - (14 + (iph->ihl * 4));
            if (data_len > 0 && (iph->protocol==75 || iph->protocol==76 ||iph->protocol==77))
            {
                printf("Data: %.*s\n\n", data_len, data);
                if (data[0] == 'n')
                {
                    if (iph->protocol == 75)
                    {
                        int nsfd = co_server(company_port['A']);
                        send(nsfd, "item from amazon nitw stall\n", 65, 0);
                        close(nsfd);
                    }
                    else if (iph->protocol == 76)
                    {
                        int nsfd = co_server(company_port['F']);
                        send(nsfd, "item from flipkart nitw stall\n", 65, 0);
                        close(nsfd);
                    }
                    else if (iph->protocol == 77)
                    {
                        int nsfd = co_server(company_port['W']);
                        send(nsfd, "item from walmart nitw stall\n", 65, 0);
                        close(nsfd);
                    }
                }
            }
            
        }
    }
    close(sock_raw);
}

int main()
{
    pthread_t th[2];
    pthread_create(&th[0], NULL, tcp_capture, NULL);
    pthread_create(&th[1], NULL, capture_outgoing_packets, NULL);
    while (1)
    {
        sleep(10);
        for (auto it = company_port.begin(); it != company_port.end(); ++it)
        {
            cout << it->first << " " << it->second << endl;
        }
    };
    return 0;
}