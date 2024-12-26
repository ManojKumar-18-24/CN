#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    struct ip *ip_hdr = (struct ip *)(packet + 14); // Assuming Ethernet header is 14 bytes
    struct tcphdr *tcp_hdr = (struct tcphdr *)(packet + 14 + (ip_hdr->ip_hl * 4));

    // Check if the packet has SYN flag set
    if (tcp_hdr->syn && !tcp_hdr->ack) {
        printf("Received SYN packet from IP: %s\n", inet_ntoa(ip_hdr->ip_src));
    }
}

int main() {
    char dev[] = "wlp0s20f3"; // Replace with your interface name
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;
    char filter_exp[] = "tcp[tcpflags] & tcp-syn != 0"; // Filter for SYN packets
    bpf_u_int32 net;

    // Open the network device for packet capture
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return 2;
    }

    // Compile the filter expression
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    // Set the filter
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    printf("Starting packet capture on interface %s...\n", dev);

    // Start packet capture loop
    pcap_loop(handle, 0, packet_handler, NULL);

    // Cleanup
    pcap_freecode(&fp);
    pcap_close(handle);

    return 0;
}
