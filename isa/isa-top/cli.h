#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>

// network headers
#include <pcap/pcap.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netinet/udp.h>
#include <netinet/icmp6.h>

// threading imports
#include <pthread.h>

#include "error.h"
#include "global.h"


CLIArguments parse_arguments(int argc, char *argv[]);

#endif // CLI_H
