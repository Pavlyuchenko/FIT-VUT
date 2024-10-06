/**
 * ISA Project - Application gathering statistics about network traffic
 * Description: isa-top is an application that listens on specified interface
 * and outputs network traffic statistics. Author: Michal Pavlíček xpavlim00
 * Note: Some code was taken from my IPK Project 2: A packet sniffer.
 */

#include "isa-top.h"

char errbuff[PCAP_ERRBUF_SIZE];
pcap_t *packet_capture;

void print_timestamp(time_t sec, time_t usec) {
    struct tm *timeinfo;
    timeinfo = localtime(&sec);

    char *timestamp = (char *)malloc(30);
    if (timestamp == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(1);
    }

    strftime(timestamp, 30, "%Y-%m-%dT%H:%M:%S", timeinfo);

    fprintf(stderr, "timestamp: %s.%06ldZ\n", timestamp, usec);
}

void print_hex_data(const struct pcap_pkthdr *header,
                    const uint8_t *packet_start) {
    printf("\n");

    for (unsigned int i = 0; i < header->caplen; i++) {
        // prints the offset
        if (i % 16 == 0) {
            printf("0x%04x: ", i);
        }

        // prints the data in hex
        printf("%02x ", packet_start[i]);

        // if at the end of the line
        if ((i + 1) % 16 == 0) {
            // print the ASCII representation of the data
            for (unsigned int j = i - 15; j <= i; j++) {
                if (packet_start[j] >= 32 && packet_start[j] <= 126) {
                    printf("%c", packet_start[j]);
                } else {
                    printf(".");
                }

                if (j % 8 == 7) {
                    printf(" ");
                }
            }

            printf("\n");
        }
    }

    // print the rest of the data
    if (header->caplen % 16 != 0) {
        // fill in the spaces
        for (unsigned int i = header->caplen; i % 16 != 0; i++) {
            printf("   ");
        }

        // print the ASCII representation of the data
        for (unsigned int i = header->caplen - (header->caplen % 16);
             i < header->caplen; i++) {
            if (packet_start[i] >= 32 && packet_start[i] <= 126) {
                printf("%c", packet_start[i]);
            } else {
                printf(".");
            }

            if (i % 8 == 7) {
                printf(" ");
            }
        }

        printf("\n");
    }
}

void packet_handler(uint8_t *args, const struct pcap_pkthdr *header,
                    const uint8_t *packet) {
    (void)(args); // unused

    // print which capture number we are on from args
    static int packet_number = 1;
    printf("Packet number %d:\n", packet_number++);
    printf("--------------------\n");

    // so that we can print the packet data in hex later
    const uint8_t *packet_start = packet;

    // converts to RFC3339
    print_timestamp(header->ts.tv_sec, header->ts.tv_usec);

    // only support Ethernet frames, which are 14 bytes long
    int ETHERNET_HEADER_LENGTH = 14;

    struct ether_header *ethernet_header = (struct ether_header *)packet;
    packet += ETHERNET_HEADER_LENGTH; // skip the Ethernet header

    printf("src MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           ethernet_header->ether_shost[0], ethernet_header->ether_shost[1],
           ethernet_header->ether_shost[2], ethernet_header->ether_shost[3],
           ethernet_header->ether_shost[4], ethernet_header->ether_shost[5]);
    printf("dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           ethernet_header->ether_dhost[0], ethernet_header->ether_dhost[1],
           ethernet_header->ether_dhost[2], ethernet_header->ether_dhost[3],
           ethernet_header->ether_dhost[4], ethernet_header->ether_dhost[5]);
    printf("frame length: %d bytes\n", header->len);

    if (ntohs(ethernet_header->ether_type) == ETH_P_IP) {
        struct ip *ip_header = (struct ip *)packet;
        // get how long the IP header is
        int IP_HEADER_LENGTH = ip_header->ip_hl * 4;

        printf("src IP: %s\n", inet_ntoa(ip_header->ip_src));
        printf("dst IP: %s\n", inet_ntoa(ip_header->ip_dst));

        packet += IP_HEADER_LENGTH; // skip the IP header

        if (ip_header->ip_p == IPPROTO_TCP) {
            struct tcphdr *tcp_header = (struct tcphdr *)packet;

            printf("src port: %d\n", ntohs(tcp_header->source));
            printf("dst port: %d\n", ntohs(tcp_header->dest));
        } else if (ip_header->ip_p == IPPROTO_UDP) {
            struct udphdr *udp_header = (struct udphdr *)packet;

            printf("src port: %d\n", ntohs(udp_header->source));
            printf("dst port: %d\n", ntohs(udp_header->dest));
        } else if (ip_header->ip_p == IPPROTO_ICMP) {
            struct icmphdr *icmp_header = (struct icmphdr *)packet;

            printf("ICMP type: %d\n", icmp_header->type);
            printf("ICMP code: %d\n", icmp_header->code);
        }
    } else if (ntohs(ethernet_header->ether_type) == ETHERTYPE_ARP) {
        struct ether_arp *arp_header = (struct ether_arp *)packet;

        printf("src IP: %d.%d.%d.%d\n", arp_header->arp_spa[0],
               arp_header->arp_spa[1], arp_header->arp_spa[2],
               arp_header->arp_spa[3]);
        printf("dst IP: %d.%d.%d.%d\n", arp_header->arp_tpa[0],
               arp_header->arp_tpa[1], arp_header->arp_tpa[2],
               arp_header->arp_tpa[3]);
        printf("ARP opcode: %d\n", ntohs(arp_header->arp_op));
    } else if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IPV6) {
        struct ip6_hdr *ip6_header = (struct ip6_hdr *)packet;

        printf("src IP: %s\n", inet_ntop(AF_INET6, &ip6_header->ip6_src,
                                         errbuff, INET6_ADDRSTRLEN));
        printf("dst IP: %s\n", inet_ntop(AF_INET6, &ip6_header->ip6_dst,
                                         errbuff, INET6_ADDRSTRLEN));

        packet += sizeof(struct ip6_hdr); // skip the IPv6 header

        if (ip6_header->ip6_ctlun.ip6_un1.ip6_un1_nxt == IPPROTO_ICMPV6) {
            struct icmp6_hdr *icmp6_header = (struct icmp6_hdr *)packet;

            printf("ICMPv6 type: %d\n", icmp6_header->icmp6_type);
            printf("ICMPv6 code: %d\n", icmp6_header->icmp6_code);
        }
    } else {
        fprintf(stderr, "Unsupported protocol\n");
        return;
    }

    print_hex_data(header, packet_start);

    printf("--------------------\n");
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
    cli_args.interval = 1;     // how often are stats updated

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
    if (!cli_args.interface) {
        fprintf(stderr, "ERROR: No interface was specified.\nRun ./isa-top -i "
                        "to get list of available interfaces.\n");
        exit(1);
    }

    return cli_args;
}

void quit_app(int signal) {
    pcap_close(packet_capture);

    printf("Exiting due to signal %d...\n", signal);
    exit(0);
}

void *capture_packets(void *arg) {
	CLIArguments *arguments = (CLIArguments *)arg;
	
    printf("Heyo from capture thread!\n");
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
	char filter[] = "ip or ip6"; // based on discussion, only need to listen for IPv4 and IPv6
    if (pcap_compile(packet_capture, &fp, filter, 0, net) == PCAP_ERROR) {
        fprintf(stderr, "Error: %s with %s\n", pcap_geterr(packet_capture),
                "filter");
        exit(1);
    }
    if (pcap_setfilter(packet_capture, &fp) == PCAP_ERROR) {
        fprintf(stderr, "Error: %s\n", pcap_geterr(packet_capture));
        exit(1);
    }

    // listen for n packets
    // TODO: Fix magic nubmer
    pcap_loop(packet_capture, -1, packet_handler, NULL);

    pcap_close(packet_capture);

    return NULL;
}

void *display_stats() {
    printf("Stats called for duty!\n");

	while (1) {
		sleep(1);

		printf("Printing statistics...\n");
	}

    return NULL;
}

int main(int argc, char *argv[]) {
    CLIArguments arguments = parse_arguments(argc, argv);

    pthread_t capture_thread, stats_thread;

    if (pthread_create(&capture_thread, NULL, capture_packets, (void *) &arguments) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_create./n");
        exit(1);
    }
    if (pthread_create(&stats_thread, NULL, display_stats, NULL) != 0) {
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
