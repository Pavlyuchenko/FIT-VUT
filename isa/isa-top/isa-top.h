/**
 * IPK Project 2 - FIT VUT
 * Description: A packet (frame) sniffer that can filter packets based on the user provided arguments.
 * Author: Michal Pavlíček (xpavlim00)
 * Date: 2024-04-21
 */

#ifndef IPK_SNIFFER_H
#define IPK_SNIFFER_H

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

/**
 *  running the script
 * ./ipk-sniffer
 *      [-i interface | --interface interface]
 *      {-p|--port-source|--port-destination port [--tcp|-t] [--udp|-u]}
 *      [--arp]
 *      [--icmp4]
 *      [--icmp6]
 *      [--igmp]
 *      [--mld]
 *      {-n num}
 */
typedef struct CLIArguments
{
    char *interface;
	char sort;
	int interval;
} CLIArguments;

/**
 * A callback function that is called for each packet captured.
 * It prints the packet data in a human-readable format.
 *
 * @param args uint8_t*
 * @param header const struct pcap_pkthdr*
 * @param packet const uint8_t*
 * @return void
 */
void packet_handler(uint8_t *args, const struct pcap_pkthdr *header, const uint8_t *packet);

/**
 * Parse the command line arguments.
 *
 * @param argc int
 * @param argv char**
 * @return CLIArguments
 */
CLIArguments parse_arguments(int argc, char *argv[]);

/**
 * Print the packet data in hex format.
 * The format is:
 * 0x0000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ........ ........
 *
 * @param header const struct pcap_pkthdr*
 * @param packet_start const uint8_t*
 * @return void
 */
void print_hex_data(const struct pcap_pkthdr *header, const uint8_t *packet_start);

/**
 * Print the timestamp in RFC3339 format.
 *
 * @param sec time_t
 * @param usec time_t
 */
void print_timestamp(time_t sec, time_t usec);

/**
 * Create a filter based on the user provided arguments.
 * Returns a string such as "tcp src port 80" that can be used in pcap_compile and pcap_setfilter.
 *
 * @param arguments CLIArguments
 * @return char*
 */
char *create_filter(CLIArguments arguments);

/**
 * Quit the application when a signal is received.
 *
 * @param signal int
 */
void quit_app(int signal);

#endif // IPK_SNIFFER_H

