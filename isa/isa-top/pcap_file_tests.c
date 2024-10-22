/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 *
 * Description: This file takes pcap files and uses them to
 *				simulate network traffic. It is then
 *				checked against Wireshark outputs for the
 *				same files. No testing library used,
 *				besides <assert.h>.
 */

#include "error.h"
#include "global.h"
#include "linked_list.h"
#include "packet_handler.h"
#include "stats.h"
#include <assert.h>
#include <pcap/pcap.h>
#include <string.h>

#define TEST_PCAP_FILE1 "icmp-data.pcap"
#define TEST_PCAP_FILE2 "test_capture.pcap"

// Helper function to count nodes in the linked list
int count_nodes() {
    int count = 0;
    Node *current = llist->head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// Helper function to check if a protocol exists in the linked list
bool protocol_exists(const char *protocol) {
    Node *current = llist->head;
    while (current != NULL) {
        if (strcmp(current->data->protocol, protocol) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Helper function to check if a port exists in the linked list
bool port_exists(uint16_t port) {
    Node *current = llist->head;
    while (current != NULL) {
        if (current->data->src_port == port ||
            current->data->dst_port == port) {
            return true;
        }
        current = current->next;
    }
    return false;
}

char sort = 'b';
void init_test_context() {
    app_context.cli_args.interface = "eth0";
    app_context.cli_args.sort = sort;
    app_context.cli_args.interval = 1;
    app_context.cli_args.cumulative = 0;
}

pcap_t *handle;
int count_packets = 0;
// initializes everything required to save data
void init_test(char *file_name) {
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr header;
    const u_char *packet;
    count_packets = 0;

    init_test_context();

    // Open the pcap file
    handle = pcap_open_offline(file_name, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open pcap file %s: %s\n", file_name, errbuf);
        return;
    }

    // Initialize the linked list
    init_llist();

    // Read and process each packet from the file
    while ((packet = pcap_next(handle, &header)) != NULL) {
        packet_handler(NULL, &header, packet);
        count_packets++;
    }
}

// tests for example pcap file
void test_icmp() {
    init_test(TEST_PCAP_FILE1);

    // the icmp pcap file was taken from Moodle
    // it contains 38 packets
    // out of which 20 is icmpv6, and 18 icmp
    // the most communication occured between hosts
    // 147.229.12.101 and 198.57.179.99, which sent
    // 14 packets totaling 1372 bytes. second communication
    // was also icmpv4, which sent 392 bytes.
    // over icmpv6, one packet was sent per communication,
    // each 86 bytes

    // assert all packets captured
    assert(llist->head != NULL);
    assert(count_packets == 38);

    CommunicationInfo *data = llist->head->data;
    // assert most communicating host is correct
    assert(strcmp(data->src_ip, "147.229.12.101") == 0);
    assert(strcmp(data->dst_ip, "198.57.179.99") == 0);
    assert(strcmp(data->protocol, "icmp") == 0);

    assert(data->packets_sent_Rx + data->packets_sent_Tx == 14);
    assert(data->Tx + data->Rx == 1372);

    CommunicationInfo *second = llist->head->next->data;
    assert(strcmp(second->protocol, "icmp") == 0);
    assert(second->Rx + second->Tx == 392);

    Node *curr_node = llist->head;
    int icmpv6_count = 0;
    while (curr_node != NULL) {
        if (strcmp(curr_node->data->protocol, "icmpv6") == 0) {
            assert(curr_node->data->Rx + curr_node->data->Tx == 86);
            icmpv6_count++;
        }
        curr_node = curr_node->next;
    }
    assert(icmpv6_count == 20);

    printf("icmp-data.pcap passed.\n");

    // Clean up
    pcap_close(handle);
}

// tests for example random network traffic
void test_arbitrary_capture() {
    init_test(TEST_PCAP_FILE2);

    // this pcap is a random communication
    // captured on my network including all
    // sorts of packets. there is udp and tcp
    // protocols present, some DNS and HTTPS
    // packets and there is 20 unique
    // communications

    // ensure there are common protocols
    assert(protocol_exists("udp"));
    assert(protocol_exists("tcp"));

    // ensure DNS and HTTPS communication is there
    assert(port_exists(53));
    assert(port_exists(443));

    assert(count_nodes() == 20);

    // ensure sorting by bytes works correctly
    assert(strcmp(llist->head->data->src_ip, "192.168.8.126") == 0);
    assert(strcmp(llist->head->data->dst_ip, "23.212.110.8") == 0);
    assert(llist->head->data->Rx + llist->head->data->Tx >= 6000);
    assert(strcmp(llist->head->data->protocol, "udp") == 0);
    assert(llist->head->data->packets_sent_Rx +
               llist->head->data->packets_sent_Tx ==
           6);

    Node *curr_node = llist->head;
    int last_bytes;
    while (curr_node->next != NULL) {
        last_bytes = curr_node->data->Rx + curr_node->data->Tx;
        curr_node = curr_node->next;

        assert(last_bytes >= curr_node->data->Rx + curr_node->data->Tx);
    }
    // switch sorting to packets
    sort = 'p';
    init_test(TEST_PCAP_FILE2);

    // ensure sorting by packets works correctly
    assert(strcmp(llist->head->data->src_ip, "192.168.8.126") == 0);
    assert(strcmp(llist->head->data->dst_ip, "34.36.57.103") == 0);
    assert(strcmp(llist->head->data->protocol, "tcp") == 0);
    assert(llist->head->data->packets_sent_Rx +
               llist->head->data->packets_sent_Tx ==
           16);

    curr_node = llist->head;
    int last_packets;
    while (curr_node->next != NULL) {
        last_packets =
            curr_node->data->packets_sent_Rx + curr_node->data->packets_sent_Tx;
        curr_node = curr_node->next;

        assert(last_packets >= curr_node->data->packets_sent_Rx +
                                   curr_node->data->packets_sent_Tx);
    }

    printf("Arbitrary pcap test_capture.pcap passed.\n");

    pcap_close(handle);
}

int main() {
	printf("Hello");
    test_icmp();
    test_arbitrary_capture();
    printf("All tests passed\n");
    return 0;
}
