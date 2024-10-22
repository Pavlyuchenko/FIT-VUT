/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 *
 * Description: functions for handling cli arguments
 */

#ifndef CLI_H
#define CLI_H

#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// network headers
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/ether.h>
#include <netinet/icmp6.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pcap/pcap.h>

// threading imports
#include <pthread.h>

#include "error.h"
#include "global.h"

CLIArguments parse_arguments(int argc, char *argv[]);

#endif // CLI_H
