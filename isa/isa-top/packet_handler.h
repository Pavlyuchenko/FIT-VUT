#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

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
#include <stdint.h>
#include <stdlib.h>

#include "stats.h"
#include "cli.h"
#include "error.h"

void packet_handler(uint8_t *args, const struct pcap_pkthdr *header,
                    const uint8_t *packet);

void *capture_packets(void *arg);

#endif // PACKET_HANDLER_H
