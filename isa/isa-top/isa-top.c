/**
 * ISA Project - Application gathering statistics about network traffic
 * @Description: isa-top is an application that listens on specified interface
 *				 and outputs network traffic statistics.
 * @Author: Michal Pavlíček xpavlim00
 * @Note: Some starter code was taken from my IPK Project 2: A packet sniffer.
 */

#include "isa-top.h"

char errbuff[PCAP_ERRBUF_SIZE];
pcap_t *packet_capture;

// variables for thread shared memory
int packet_count = 0;
int bites_sent = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// struct for holding information about a communication between two ip addresses
typedef struct CommunicationInfo {
    // info about hosts
    char *src_ip;
    char *dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    char *protocol;

    // info about their communication (statistics)
    long int Rx;           // received data
    long int Tx;           // transmitted data
    long int packets_sent; // packets sent between these two IPs
} CommunicationInfo;

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
    char *src_ip;
    char *dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    char *protocol;

    if (ntohs(ethernet_header->ether_type) == ETH_P_IP) {
        struct ip *ip_header = (struct ip *)packet;
        // get how long the IP header is
        int IP_HEADER_LENGTH = ip_header->ip_hl * 4;

        src_ip = strdup(inet_ntoa(ip_header->ip_src));
        dst_ip = strdup(inet_ntoa(ip_header->ip_dst));

        packet += IP_HEADER_LENGTH; // skip the IP header

        if (ip_header->ip_p == IPPROTO_TCP) {
            protocol = "tcp";
            struct tcphdr *tcp_header = (struct tcphdr *)packet;

            src_port = ntohs(tcp_header->source);
            dst_port = ntohs(tcp_header->dest);
        } else if (ip_header->ip_p == IPPROTO_UDP) {
            protocol = "udp";
            struct udphdr *udp_header = (struct udphdr *)packet;

            src_port = ntohs(udp_header->source);
            dst_port = ntohs(udp_header->dest);
        }
    } else if (ntohs(ethernet_header->ether_type) == ETHERTYPE_ARP) {
		struct ether_arp *arp_header = (struct ether_arp *)packet;
		printf("src MAC: %s\n", ether_ntoa((struct ether_addr *)arp_header->arp_sha));
		printf("dst MAC: %s\n", ether_ntoa((struct ether_addr *)arp_header->arp_tha));
		return;  // skip further processing for ARP packets
	} else if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IPV6) {
        struct ip6_hdr *ip6_header = (struct ip6_hdr *)packet;

        printf("src IP: %s\n", inet_ntop(AF_INET6, &ip6_header->ip6_src,
                                         errbuff, INET6_ADDRSTRLEN));
        printf("dst IP: %s\n", inet_ntop(AF_INET6, &ip6_header->ip6_dst,
                                         errbuff, INET6_ADDRSTRLEN));

        packet += sizeof(struct ip6_hdr); // skip the IPv6 header

    } else {
        fprintf(stderr, "Unsupported protocol %d\n",
                ethernet_header->ether_type);
        return;
    }

    pthread_mutex_lock(&mutex);
    packet_count++;
	bites_sent += packet_len_bytes * 8 / 1000; // kbits
    pthread_mutex_unlock(&mutex);
}

CLIArguments parse_arguments(int argc, char *argv[]) {
    /*  ./ipk-sniffer
     *      [-i interface]
     *      {-s sort by}
     *      {-t interval for stat update}
     *      {-h show this help}
     */
    CLIArguments cli_args;
    int opt;
    cli_args.interface = NULL; // which interface is listened on
    cli_args.sort = 'b';       // either b or p meaning bytes/packets per second
    cli_args.interval = 1;     // how often stats are updated

    // argument parsing done with help from:
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    while ((opt = getopt(argc, argv, "i:s:t:h")) != -1) {
        long temp_interval;
        switch (opt) {
        case 'i':
            cli_args.interface = optarg;
            break;
        case 's':
            cli_args.sort = optarg[0];

            if (!(cli_args.sort == 'b' || cli_args.sort == 'p')) {
                fprintf(stderr,
                        "ERROR: Only valid options for -s are b or p.\n");
            }
            break;
        case 't':
            char *endptr;
            temp_interval = strtol(optarg, &endptr, 10);

            if (*endptr != '\0' || temp_interval <= 0) {
                fprintf(stderr,
                        "ERROR: Invalid interval '%s'. Must be a positive "
                        "integer.\n",
                        optarg);
                exit(1);
            }
            cli_args.interval = (int)temp_interval;
            break;
        case 'h':
            printf("isa-top: displays network traffic on an interface by host. "
                   "Created for course ISA @ FIT VUT."
                   "\n\nSynopsis: "
                   "./ipk-sniffer\n\t[-i interface]\n\t{-s sort by b|p}\n\t{-t "
                   "update interval}\n\t{-h show this help}\n\n"
                   "\t-h\t\tdisplays this message\n\t-i\t\tdisplays list of "
                   "available "
                   "interfaces\n\t-i "
                   "interface\t\tlisten on this interfac\n\t-s "
                   "sort\t\tspecifies the sorting of displayed statistics, "
                   "either by bytes or packets.\n\t-t interval\t\tsets the "
                   "update interval of statistics\n\nisa-top, copyright "
                   "Michal Pavlíček <xpavlim00@stud.fit.vutbr.cz, "
                   "michaelg.pavlicek@gmail.com>, 2024.\n");
            exit(0);
            break;
        case '?':
            if (optopt == 'i') {
                pcap_if_t *devices = NULL;
                if (pcap_findalldevs(&devices, errbuff) != PCAP_ERROR) {
                    if (devices != NULL) {
                        pcap_if_t *curr;
                        for (curr = devices; curr; curr = curr->next) {
                            if ((curr->flags & PCAP_IF_UP) &&
                                (curr->flags & PCAP_IF_RUNNING)) {
                                printf("%s\n", curr->name);
                            }
                        }
                    } else {
                        fprintf(stderr, "No devices were found\n");
                    }

                    pcap_freealldevs(devices);
                } else {
                    fprintf(stderr, "Error: %s\n", errbuff);
                }

                exit(0);
            }

            if (optopt == 's' || optopt == 't') {
                fprintf(stderr, "ERROR: Option -%c requires an argument.\n",
                        optopt);
                exit(1);
            } else if (isprint(optopt)) {
                fprintf(stderr, "ERROR: Unknown option -%c.\n", optopt);
                exit(1);
            } else {
                fprintf(stderr, "ERROR: Unknown character.\n");
                exit(1);
            }
            break;
        }
    }
    if (!cli_args.interface && optopt != 't') {
        fprintf(stderr, "ERROR: No interface was specified.\nRun ./isa-top -i "
                        "to get list of available interfaces.\n");
        exit(1);
    }

    return cli_args;
}

void quit_app(int signal) {
    pcap_close(packet_capture);

    printf("\nExiting due to signal %d...\n", signal);
    exit(0);
}

void *capture_packets(void *arg) {
    CLIArguments *arguments = (CLIArguments *)arg;

    bpf_u_int32 net, mask;
    if (pcap_lookupnet(arguments->interface, &net, &mask, errbuff) ==
        PCAP_ERROR) {
        fprintf(stderr, "Error: %s\n", errbuff);
        exit(1);
    }

    packet_capture =
        pcap_open_live(arguments->interface, 65535, 1, 1000, errbuff);
    if (packet_capture == NULL) {
        fprintf(stderr, "Error: %s\n", errbuff);
        exit(1);
    }

    // this handles Ctrl+C and other signals, such that we can close the pcap
    // session
    signal(SIGINT, quit_app);
    signal(SIGTERM, quit_app);
    signal(SIGQUIT, quit_app);

    // make sure the device provides Ethernet headers
    if (pcap_datalink(packet_capture) != DLT_EN10MB) {
        fprintf(stderr, "The chosen device does not provide Ethernet headers. "
                        "Choose a different one.\n");
        exit(1);
    }

    // filter creation (for user provided arguments)
    struct bpf_program fp;
    char filter[] = "ip or ip6"; // based on discussion, only need to listen for
                                 // IPv4 and IPv6
    if (pcap_compile(packet_capture, &fp, filter, 0, net) == PCAP_ERROR) {
        fprintf(stderr, "Error: %s with %s\n", pcap_geterr(packet_capture),
                "filter");
        exit(1);
    }
    if (pcap_setfilter(packet_capture, &fp) == PCAP_ERROR) {
        fprintf(stderr, "Error: %s\n", pcap_geterr(packet_capture));
        exit(1);
    }

    // listen for packets indefinitely
    pcap_loop(packet_capture, -1, packet_handler, NULL);

    pcap_close(packet_capture);

    return NULL;
}

void *display_stats(void *arg) {
    CLIArguments *arguments = (CLIArguments *)arg;

    while (1) {
        sleep(1);

        pthread_mutex_lock(&mutex);
        printf("Packets sent: %d\nBites sent: %d\n\n", packet_count, bites_sent);
        packet_count = 0;
		bites_sent = 0;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    CLIArguments arguments = parse_arguments(argc, argv);

    pthread_t capture_thread, stats_thread;
	pthread_mutex_init(&mutex, NULL);

    if (pthread_create(&capture_thread, NULL, capture_packets,
                       (void *)&arguments) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_create./n");
        exit(1);
    }
    if (pthread_create(&stats_thread, NULL, display_stats,
                       (void *)&arguments) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_create./n");
        exit(1);
    }

    if (pthread_join(capture_thread, NULL) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_join./n");
        exit(1);
    }
    if (pthread_join(stats_thread, NULL) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_join./n");
        exit(1);
    }

    return 0;
}
