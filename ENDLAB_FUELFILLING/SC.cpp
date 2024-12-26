#include <bits/stdc++.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;
#define BUFFER_SIZE 65536

vector<pair<int,int>> store;

void printPacketDetails(const struct iphdr *ipHeader, const struct tcphdr *tcpHeader, const uint8_t *payload, int payloadSize) {
    // Extract source and destination IP addresses
    struct in_addr srcAddr, destAddr;
    srcAddr.s_addr = ipHeader->saddr;
    destAddr.s_addr = ipHeader->daddr;

    cout << "Outgoing TCP Packet:" <<endl;
    cout << "Source IP: " << inet_ntoa(srcAddr)
              << ", Destination IP: " << inet_ntoa(destAddr) << endl;
    cout << "Source Port: " << ntohs(tcpHeader->source)
              << ", Destination Port: " << ntohs(tcpHeader->dest) <<endl;

    // Print raw data in payload as a string
    if (payloadSize > 0) {
        cout << "Payload Data: ";
        for (int i = 0; i < payloadSize; ++i) {
            cout << (char)payload[i];  // Print character data
        }
        cout <<endl;

    } else {
        cout << "No data found in payload." << endl;
    }

cout << "----------------------------------------" << endl;
}

bool isLocalIP(uint32_t ip) {
    // Check if the IP is the loopback address 127.0.0.1
    if (ip == inet_addr("127.0.0.1")) {
        return true;
    }

    // Get the local machine's IP address
    char hostname[1024];
    struct hostent *host;
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return false;
    }
    
    host = gethostbyname(hostname);
    if (host == nullptr) {
        perror("gethostbyname");
        return false;
    }

    // Loop through all the IP addresses returned by gethostbyname
    for (int i = 0; host->h_addr_list[i] != nullptr; i++) {
        struct in_addr localAddr;
        memcpy(&localAddr, host->h_addr_list[i], sizeof(struct in_addr));

        // If the source IP matches any of the local IP addresses
        if (ip == localAddr.s_addr) {
            return true;
        }
    }

    return false;
}

bool check(const char* filename, const std::vector<std::pair<int, int>>& data) {
    int fd = open("result.txt", O_RDONLY);
    if (fd < 0) {
        std::cerr << "Error opening file: " << strerror(errno) << "\n";
        return false;
    }

    char buffer[1024];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (bytesRead < 0) {
        std::cerr << "Error reading file: " << strerror(errno) << "\n";
        return false;
    }
    buffer[bytesRead] = '\0'; // Null-terminate the buffer

    int idx = 0;
    char* token = strtok(buffer, "\n");
    while (token != nullptr && idx < data.size()) {
        int first, second;
        if (sscanf(token, "%d %d", &first, &second) != 2 || data[idx] != std::make_pair(first, second)) {
            return false;
        }
        token = strtok(nullptr, "\n");
        ++idx;
    }

    return token == nullptr && idx == data.size(); // Check for size mismatch
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];

    // Create a raw socket for IP packets
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    std::cout << "TCP tracer started, capturing outgoing packets with data payloads..." << std::endl;
    int i=0;
    while (true) {
        // Receive packets
        ssize_t packetSize = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (packetSize < 0) {
            perror("Packet receive failed");
            break;
        }

        // Extract IP header
        struct iphdr *ipHeader = (struct iphdr*)buffer;
        if (ipHeader->protocol != IPPROTO_TCP) {
            continue;  // Ignore non-TCP packets
        }

        // Calculate the IP header length
        size_t ipHeaderLen = ipHeader->ihl * 4;

        // Extract TCP header
        struct tcphdr *tcpHeader = (struct tcphdr*)(buffer + ipHeaderLen);

        // Calculate TCP header length
        size_t tcpHeaderLen = tcpHeader->doff * 4;

        // Only capture outgoing packets from this machine
        if (!isLocalIP(ipHeader->saddr)) {
            continue;  // Skip if the source IP is not the local IP or loopback
        }

        // Extract payload starting after the IP and TCP headers
        uint8_t *payload = (uint8_t*)(buffer + ipHeaderLen + tcpHeaderLen);
        int payloadSize = packetSize - ipHeaderLen - tcpHeaderLen;

        // Print packet and payload details
        printPacketDetails(ipHeader, tcpHeader, payload, payloadSize);
        if(i==10)
        {

        }
    }

    close(sockfd);
    return 0;
}