#include "stats.h"
#include "error.h"
#include "linked_list.h"
#include <string.h>

// variables for thread shared memory
int packet_count = 0;
int bites_sent = 0;

CommunicationInfo *init_communication() {
    CommunicationInfo *c_i = malloc(sizeof(*c_i));
    if (c_i == NULL) {
		throw_error("CommunicationInfo", ERR_MALLOC);
    }
    strcpy(c_i->src_ip, "");
    strcpy(c_i->dst_ip, "");
    strcpy(c_i->protocol, "");

    c_i->Rx = 0;
    c_i->Tx = 0;
    c_i->packets_sent = 0;

    return c_i;
}

void insert_communication(CommunicationInfo *c_info) {
	Node *node = get_node(c_info);

	if (node->data == NULL) {
		node->data = c_info;
		node->data->packets_sent++;
		return;
	}
	
	add_communications(node->data, c_info);

	update_node_position(node, true);
}

CommunicationInfo *add_communications(CommunicationInfo *first,
                                      CommunicationInfo *second) {
    first->packets_sent += second->packets_sent;

    if (strcmp(first->src_ip, second->src_ip) == 0 &&
        first->src_port == second->src_port) {
        first->Tx += second->Tx;
        first->Rx += second->Rx;

        // TODO: free() second
        return first;
    }

    first->Tx += second->Rx;
    first->Rx += second->Tx;
	first->packets_sent += second->packets_sent;

    return first;
}

void *display_stats() {
    while (1) {
        sleep(1);

        pthread_mutex_lock(&app_context.mutex);
		print_llist(10);
        pthread_mutex_unlock(&app_context.mutex);
    }

    return NULL;
}
