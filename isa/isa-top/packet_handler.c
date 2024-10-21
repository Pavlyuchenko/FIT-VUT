#include "packet_handler.h"
#include "error.h"
#include "stats.h"
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

void packet_handler(uint8_t *args, const struct pcap_pkthdr *header,
                    const uint8_t *packet) {
    (void)(args); // argument 'args' is unused

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
        } else if (ip_header->ip_p == IPPROTO_ICMP) {
            strcpy(protocol, "icmp");
        } else {
            struct protoent *proto;

            proto = getprotobynumber(ip_header->ip_p);
            if (proto != NULL) {
                strncpy(protocol, proto->p_name, MAX_PROTO_LEN - 1);
                protocol[MAX_PROTO_LEN - 1] = '\0';
            } else {
                snprintf(protocol, MAX_PROTO_LEN, "Unknown (%d)",
                         ip_header->ip_p);
            }
        }
    } else if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IPV6) {
        struct ip6_hdr *ip6_header = (struct ip6_hdr *)packet;

        // Convert IPv6 addresses to string representation
        inet_ntop(AF_INET6, &ip6_header->ip6_src, src_ip, sizeof(src_ip));
        inet_ntop(AF_INET6, &ip6_header->ip6_dst, dst_ip, sizeof(dst_ip));

        packet += sizeof(struct ip6_hdr); // skip the IPv6 header

        // Check the next header type
        uint8_t next_header = ip6_header->ip6_nxt;

        if (next_header == IPPROTO_TCP) {
            strcpy(protocol, "tcp");
            struct tcphdr *tcp_header = (struct tcphdr *)packet;

            src_port = ntohs(tcp_header->source);
            dst_port = ntohs(tcp_header->dest);
        } else if (next_header == IPPROTO_UDP) {
            strcpy(protocol, "udp");
            struct udphdr *udp_header = (struct udphdr *)packet;

            src_port = ntohs(udp_header->source);
            dst_port = ntohs(udp_header->dest);
        } else if (next_header == IPPROTO_ICMPV6) {
            strcpy(protocol, "icmpv6");
        } else {
            struct protoent *proto;

            proto = getprotobynumber(next_header);
            if (proto != NULL) {
                strncpy(protocol, proto->p_name, MAX_PROTO_LEN - 1);
                protocol[MAX_PROTO_LEN - 1] = '\0';
            } else {
                snprintf(protocol, MAX_PROTO_LEN, "Unknown (%d)", next_header);
            }
        }

    } else {
        // fprintf(stderr, "Unsupported protocol %d\n",
        // ethernet_header->ether_type);
        return;
    }

    pthread_mutex_lock(&app_context.mutex);
    CommunicationInfo *ci = init_communication();
    strcpy(ci->src_ip, src_ip);
    strcpy(ci->dst_ip, dst_ip);
    strcpy(ci->protocol, protocol);

    ci->src_port = src_port;
    ci->dst_port = dst_port;

    ci->Rx = packet_len_bytes;
    ci->Tx = 0;
    ci->packets_sent_Tx = 1;
    insert_communication(ci);
    pthread_mutex_unlock(&app_context.mutex);
}

void *capture_packets(void *arg) {
    CLIArguments *arguments = (CLIArguments *)arg;

    bpf_u_int32 net, mask;
    if (pcap_lookupnet(arguments->interface, &net, &mask, errbuff) ==
        PCAP_ERROR) {
        throw_error("interface does not exist", ERR_PACKET_CAPTURE);
    }

    app_context.packet_capture =
        pcap_open_live(arguments->interface, 65535, 1, 1000, errbuff);
    if (app_context.packet_capture == NULL) {
        throw_error("openning packet capture failed", ERR_PACKET_CAPTURE);
    }

    // this handles Ctrl+C and other signals, such that we can close the pcap
    // session
    signal(SIGINT, quit_app);
    signal(SIGTERM, quit_app);
    signal(SIGQUIT, quit_app);

    // make sure the device provides Ethernet headers
    if (pcap_datalink(app_context.packet_capture) != DLT_EN10MB) {
        throw_error("interface does not provide ethernet headers",
                    ERR_PACKET_CAPTURE);
    }

    // filter creation (for user provided arguments)
    struct bpf_program fp;
    char filter[] = "ip or ip6"; // based on discussion, only need to listen for
                                 // IPv4 and IPv6
    if (pcap_compile(app_context.packet_capture, &fp, filter, 0, net) ==
        PCAP_ERROR) {

        throw_error("entered filter is wrong", ERR_PACKET_CAPTURE);
    }
    if (pcap_setfilter(app_context.packet_capture, &fp) == PCAP_ERROR) {
        throw_error("setting filter failed", ERR_PACKET_CAPTURE);
    }

    // listen for packets indefinitely
    pcap_loop(app_context.packet_capture, -1, packet_handler, NULL);

    pcap_close(app_context.packet_capture);

    return NULL;
}
