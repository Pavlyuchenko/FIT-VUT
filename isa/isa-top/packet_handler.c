#include "packet_handler.h"
#include "stats.h"

void packet_handler(uint8_t *args, const struct pcap_pkthdr *header,
                    const uint8_t *packet) {
    (void)(args); // argument 'args' is unused

    // so that we can print the packet data in hex later
    const uint8_t *packet_start = packet;

    // only support Ethernet frames, which are 14 bytes long
    int ETHERNET_HEADER_LENGTH = 14;

    struct ether_header *ethernet_header = (struct ether_header *)packet;
    packet += ETHERNET_HEADER_LENGTH; // skip the Ethernet header

    int packet_len_bytes = header->len;
    char src_ip[MAX_IP_LEN];
    char dst_ip[MAX_IP_LEN];
    uint16_t src_port = 0;
    uint16_t dst_port = 0;
    char protocol[MAX_PROTO_LEN];

    if (ntohs(ethernet_header->ether_type) == ETH_P_IP) {
        struct ip *ip_header = (struct ip *)packet;
        // get how long the IP header is
        int IP_HEADER_LENGTH = ip_header->ip_hl * 4;

        inet_ntop(AF_INET, &ip_header->ip_src, src_ip, sizeof(src_ip));
        inet_ntop(AF_INET, &ip_header->ip_dst, dst_ip, sizeof(dst_ip));

        packet += IP_HEADER_LENGTH; // skip the IP header

        if (ip_header->ip_p == IPPROTO_TCP) {
            strcpy(protocol, "tcp");
            struct tcphdr *tcp_header = (struct tcphdr *)packet;

            src_port = ntohs(tcp_header->source);
            dst_port = ntohs(tcp_header->dest);
        } else if (ip_header->ip_p == IPPROTO_UDP) {
            strcpy(protocol, "udp");
            struct udphdr *udp_header = (struct udphdr *)packet;

            src_port = ntohs(udp_header->source);
            dst_port = ntohs(udp_header->dest);
        }
    } else if (ntohs(ethernet_header->ether_type) == ETHERTYPE_ARP) {
        struct ether_arp *arp_header = (struct ether_arp *)packet;
        printf("src MAC: %s\n",
               ether_ntoa((struct ether_addr *)arp_header->arp_sha));
        printf("dst MAC: %s\n",
               ether_ntoa((struct ether_addr *)arp_header->arp_tha));
        return; // skip further processing for ARP packets
    } else if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IPV6) {
        struct ip6_hdr *ip6_header = (struct ip6_hdr *)packet;

        // printf("src IP: %s\n", inet_ntop(AF_INET6, &ip6_header->ip6_src, errbuff, INET6_ADDRSTRLEN));
        // printf("dst IP: %s\n", inet_ntop(AF_INET6, &ip6_header->ip6_dst, errbuff, INET6_ADDRSTRLEN));

        packet += sizeof(struct ip6_hdr); // skip the IPv6 header

    } else {
        fprintf(stderr, "Unsupported protocol %d\n",
                ethernet_header->ether_type);
        return;
    }

	//printf("srcIp: %s, dstIp: %s\n", src_ip, dst_ip);

    pthread_mutex_lock(&app_context.mutex);
	CommunicationInfo *ci = init_communication();
	strcpy(ci->src_ip, src_ip);
	strcpy(ci->dst_ip, dst_ip);
	strcpy(ci->protocol, protocol);

	ci->src_port = src_port;
	ci->dst_port = dst_port;

	ci->Rx = packet_len_bytes;
	ci->Tx = 0;
	ci->packets_sent = 1;
	insert_communication(ci);
    pthread_mutex_unlock(&app_context.mutex);
}

void *capture_packets(void *arg) {
    CLIArguments *arguments = (CLIArguments *)arg;

    bpf_u_int32 net, mask;
    if (pcap_lookupnet(arguments->interface, &net, &mask, errbuff) ==
        PCAP_ERROR) {
        fprintf(stderr, "Error: %s\n", errbuff);
        exit(1);
    }

    app_context.packet_capture =
        pcap_open_live(arguments->interface, 65535, 1, 1000, errbuff);
    if (app_context.packet_capture == NULL) {
        fprintf(stderr, "Error: %s\n", errbuff);
        exit(1);
    }

    // this handles Ctrl+C and other signals, such that we can close the pcap
    // session
    signal(SIGINT, quit_app);
    signal(SIGTERM, quit_app);
    signal(SIGQUIT, quit_app);

    // make sure the device provides Ethernet headers
    if (pcap_datalink(app_context.packet_capture) != DLT_EN10MB) {
        fprintf(stderr, "The chosen device does not provide Ethernet headers. "
                        "Choose a different one.\n");
        exit(1);
    }

    // filter creation (for user provided arguments)
    struct bpf_program fp;
    char filter[] = "ip or ip6"; // based on discussion, only need to listen for
                                 // IPv4 and IPv6
    if (pcap_compile(app_context.packet_capture, &fp, filter, 0, net) == PCAP_ERROR) {
        fprintf(stderr, "Error: %s with %s\n", pcap_geterr(app_context.packet_capture),
                "filter");
        exit(1);
    }
    if (pcap_setfilter(app_context.packet_capture, &fp) == PCAP_ERROR) {
        fprintf(stderr, "Error: %s\n", pcap_geterr(app_context.packet_capture));
        exit(1);
    }

    // listen for packets indefinitely
    pcap_loop(app_context.packet_capture, -1, packet_handler, NULL);

    pcap_close(app_context.packet_capture);

    return NULL;
}

