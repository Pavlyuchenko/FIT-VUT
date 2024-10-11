#include "linked_list.h"

LinkedList *llist;

void init_llist() {
    llist = malloc(sizeof(LinkedList));
    if (llist == NULL) {
        throw_error("LinkedList", ERR_MALLOC);
    }

    llist->head = NULL;

    return;
}

bool compare_communications(CommunicationInfo *one, CommunicationInfo *two) {
    if (!one || !two) {
        return false;
    }
    return (strcmp(one->src_ip, two->src_ip) == 0 &&
            strcmp(one->dst_ip, two->dst_ip) == 0 &&
            (one->src_port == two->src_port) &&
            (one->dst_port == two->dst_port)) ||
           (strcmp(one->src_ip, two->dst_ip) == 0 &&
            strcmp(one->dst_ip, two->src_ip) == 0 &&
            (one->src_port == two->dst_port) &&
            (one->dst_port == two->src_port));
}

Node *get_node(CommunicationInfo *c_info) {
    Node *curr_node = llist->head;
    Node *prev_node = NULL;

    if (curr_node == NULL) {
        curr_node = init_node();
        llist->head = curr_node;
        curr_node->data = c_info;
        curr_node->data->packets_sent = 1;
    }

    while (curr_node != NULL) {
        bool same = compare_communications(c_info, curr_node->data);

        if (same) {
            return curr_node;
        }

        prev_node = curr_node;
        curr_node = curr_node->next;
    }

    curr_node = init_node();
    prev_node->next = curr_node;
    curr_node->prev = prev_node;
    curr_node->data = NULL;

    return curr_node;
}

// order is either n->Rx + n->Tx
// or packets_sent, based on congig
// @order => 0 is transmission speed
//			 1 is packets sent
long int get_nodes_order(Node *node, bool order) {
    if (order) {
        return node->data->Rx + node->data->Tx;
    }

    return node->data->packets_sent;
}

// Assumes nodes data have changed, therefore
// looks at prev nodes until it finds its
// corresponding new place in the sorted llist
void update_node_position(Node *node, bool order) {
    if (node == NULL || node->prev == NULL) {
        return;
    }

    long int node_order = get_nodes_order(node, order);
    Node *prev_node = node->prev;

    while (prev_node != NULL &&
           get_nodes_order(prev_node, order) < node_order) {
        prev_node = prev_node->prev;
    }

    // node stays in the same place
    if ((prev_node != NULL && prev_node->next == node) ||
        (prev_node == NULL && node == llist->head)) {
        return;
    }

    // Remove node from its current position
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    node->prev->next = node->next;

    // node is new head
    if (prev_node == NULL) {
        llist->head->prev = node;
        node->next = llist->head;
        llist->head = node;
        node->prev = NULL;

        return;
    }

    // insert node at its new place
    node->next = prev_node->next;
    if (prev_node->next != NULL) {
        prev_node->next->prev = node;
    }
    node->prev = prev_node;
    prev_node->next = node;

    return;
}

double KB = 1024.0;
double MB = 1048576.0;
double GB = 1073741824.0;
char *bytes_conversion(long int bytes) {
    int max = 100;
    char *buf = malloc(max);
    if (buf == NULL) {
        return NULL;
    }

    if (bytes > GB) {
        snprintf(buf, max, "%.2f GB/s\n", bytes / GB);
    } else if (bytes > MB) {
        snprintf(buf, max, "%.2f MB/s\n", bytes / MB);
    } else if (bytes > KB) {
        snprintf(buf, max, "%.2f KB/s\n", bytes / KB);
    } else {
        snprintf(buf, max, "%ld B/s\n", bytes);
    }

    return buf;
}

void print_node_data(Node *node) {
    if (node && node->data) {
        printf("%s:%d -> %s:%d, total: %s\n", node->data->src_ip,
               node->data->src_port, node->data->dst_ip, node->data->dst_port,
               bytes_conversion(node->data->Rx + node->data->Tx));
    } else {
        printf("Node or node data is NULL\n");
    }
}

void print_llist(int count) {
    printf("\n-----------------------------------\n");
    Node *curr_node = llist->head;
    long int total_packets = 0;
    long int total_transmission_speed = 0;

    while (curr_node != NULL && count > 0) {
        print_node_data(curr_node);

        total_packets += curr_node->data->packets_sent;
        total_transmission_speed += curr_node->data->Rx + curr_node->data->Tx;

        count--;
        curr_node = curr_node->next;
    }

    printf("Total packets: %ld, Total transmission speed: %s.\n", total_packets,
           bytes_conversion(total_transmission_speed));
}

Node *init_node() {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        throw_error("Node", ERR_MALLOC);
    }

    node->next = NULL;
    node->prev = NULL;
    node->data = NULL;

    return node;
}
