#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 65536
#define DEST_IP "192.168.137.242"
#define DEST_PORT 12345

unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short *)b;
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

void process_packet(const u_char *packet, int packet_size) {
    struct iphdr *ip_header = (struct iphdr *)packet;
    int ip_header_length = ip_header->ihl * 4;

    struct udphdr *udp_header = (struct udphdr *)(packet + ip_header_length);
    char *data = (char *)(packet + ip_header_length + sizeof(struct udphdr));

    printf("Intercepted packet: %s\n", data);

    if (strcmp(data, "available") == 0) {
        int raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
        if (raw_sock < 0) {
            perror("Socket creation failed");
            return;
        }

        struct sockaddr_in dest;
        dest.sin_family = AF_INET;
        dest.sin_port = udp_header->dest;
        dest.sin_addr.s_addr = ip_header->daddr;

        if (sendto(raw_sock, packet, packet_size, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
            perror("sendto failed");
        } else {
            printf("Packet forwarded: %s\n", data);
        }

        //close(raw_sock);
    }
}

int main() {
    char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live("wlp0s20f3", BUFFER_SIZE, 1, 1000, error_buffer);
    if (!handle) {
        fprintf(stderr, "Could not open device: %s\n", error_buffer);
        return 1;
    }

    printf("Listening for packets on wlp0s20f3...\n");

    struct bpf_program filter;
    char filter_exp[128];
    sprintf(filter_exp, "udp and dst host %s and dst port %d", DEST_IP, DEST_PORT);

    if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "pcap_compile failed: %s\n", pcap_geterr(handle));
        return 1;
    }

    if (pcap_setfilter(handle, &filter) == -1) {
        fprintf(stderr, "pcap_setfilter failed: %s\n", pcap_geterr(handle));
        return 1;
    }

    struct pcap_pkthdr *header;
    const u_char *packet;
    int res;

    while ((res = pcap_next_ex(handle, &header, &packet)) >= 0) {
        if (res == 0) continue;
        process_packet(packet, header->len);
    }

    pcap_close(handle);
    return 0;
}
