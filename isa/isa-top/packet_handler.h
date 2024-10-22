/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 *
 * Description: File for packet capturing and handling.
 */

#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

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
#include <stdint.h>
#include <stdlib.h>

#include "cli.h"
#include "error.h"
#include "stats.h"

// handles and classifies captured packets
void packet_handler(uint8_t *args, const struct pcap_pkthdr *header,
                    const uint8_t *packet);

// main function for packet handling, used by threads
void *capture_packets(void *arg);

#endif // PACKET_HANDLER_H
