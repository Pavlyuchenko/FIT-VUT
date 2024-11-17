/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 */

#include "linked_list.h"
#include "global.h"

LinkedList *llist;

void init_llist() {
    if (llist != NULL) {
        free(llist);
    }

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
        curr_node->data = NULL;

        return curr_node;
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
long int get_nodes_order(Node *node) {
    if (app_context.cli_args.sort == 'b') {
        return (node->data->Rx + node->data->Tx) /
               app_context.cli_args.interval;
    }

    return (node->data->packets_sent_Rx + node->data->packets_sent_Tx) /
           app_context.cli_args.interval;
}

// Assumes nodes data have changed, therefore
// looks at prev nodes until it finds its
// corresponding new place in the sorted llist
void update_node_position(Node *node) {
    if (node == NULL || node->prev == NULL) {
        return;
    }

    long int node_order = get_nodes_order(node);
    Node *prev_node = node->prev;

    while (prev_node != NULL && get_nodes_order(prev_node) < node_order) {
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

char *bytes_conversion(long int bytes) {
    static char buf[100];
    double bits = bytes * 8;
    double Kb = 1024.0;
    double Mb = 1024.0 * Kb;
    double Gb = 1024.0 * Mb;

    if (bits >= Gb) {
        snprintf(buf, sizeof(buf), "%.1f G",
                 bits / Gb / app_context.cli_args.interval);
    } else if (bits >= Mb) {
        snprintf(buf, sizeof(buf), "%.1f M",
                 bits / Mb / app_context.cli_args.interval);
    } else if (bits >= Kb) {
        snprintf(buf, sizeof(buf), "%.1f K",
                 bits / Kb / app_context.cli_args.interval);
    } else {
        snprintf(buf, sizeof(buf), "%.1f", bits);
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

void format_ip_port(char *buffer, size_t bufsize, const char *ip,
                    uint16_t port) {
    size_t ip_len = strlen(ip);
    if (ip_len + 7 > bufsize) {
        snprintf(buffer, bufsize, "%.*s:%.5u", (int)(bufsize - 7), ip, port);
    } else {
        snprintf(buffer, bufsize, "%s:%u", ip, port);
    }
}

void print_llist(int count) {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // box around screen
    box(stdscr, 0, 0);

    // app title
    attron(A_BOLD | COLOR_PAIR(1));
    mvprintw(1, (max_x - 16) / 2, "isa-top : xpavlim00");

    // header
    mvprintw(3, 2, "Source IP:Port");
    mvprintw(3, 26, "Dest IP:Port");
    mvprintw(3, 50, "Proto");
    mvprintw(3, 57, "Rx b/s");
    mvprintw(3, 66, "Tx b/s");
    mvprintw(3, 75, "Rx p/s");
    mvprintw(3, 84, "Tx p/s");

    // horizontal line under the header
    mvhline(4, 1, ACS_HLINE, max_x - 2);

    Node *curr_node = llist->head;
    int row = 5;
    long int total_packets_rx = 0;
    long int total_packets_tx = 0;
    long int total_rx = 0;
    long int total_tx = 0;

    while (curr_node != NULL && count > 0 && row < max_y - 4) {
        if (curr_node->data) {
            char src[24], dst[24];
            format_ip_port(src, sizeof(src), curr_node->data->src_ip,
                           curr_node->data->src_port);
            format_ip_port(dst, sizeof(dst), curr_node->data->dst_ip,
                           curr_node->data->dst_port);

            mvprintw(row, 2, "%-23.23s", src);
            mvprintw(row, 26, "%-23.23s", dst);
            mvprintw(row, 50, "%-6.6s", curr_node->data->protocol);
            mvprintw(row, 57, "%-8s",
                     bytes_conversion(curr_node->data->Rx));
            mvprintw(row, 66, "%-8s",
                     bytes_conversion(curr_node->data->Tx));
            mvprintw(row, 75, "%-8ld",
                     curr_node->data->packets_sent_Rx);
            mvprintw(row, 84, "%-8ld",
                     curr_node->data->packets_sent_Tx);

            total_packets_rx += curr_node->data->packets_sent_Rx;
            total_packets_tx += curr_node->data->packets_sent_Tx;
            total_rx += curr_node->data->Rx;
            total_tx += curr_node->data->Tx;

            row++;
            count--;
        }
        curr_node = curr_node->next;
    }

    // Draw a horizontal line above the totals
    mvhline(max_y - 4, 1, ACS_HLINE, max_x - 2);

    // Print totals
    if (app_context.cli_args.cumulative) {
        mvprintw(max_y - 3, 2, "Total Rx: %s (%ld p)",
                 bytes_conversion(total_rx), total_packets_rx);
        mvprintw(max_y - 2, 2, "Total Tx: %s (%ld p)",
                 bytes_conversion(total_tx), total_packets_tx);
    } else {
        mvprintw(max_y - 3, 2, "Total Rx: %s (%ld p)",
                 bytes_conversion(total_rx),
                 total_packets_rx / app_context.cli_args.interval);
        mvprintw(max_y - 2, 2, "Total Tx: %s (%ld p)",
                 bytes_conversion(total_tx),
                 total_packets_tx / app_context.cli_args.interval);
    }

    // Print instructions
    mvprintw(max_y - 2, max_x - 26, "Press 'Ctrl + C' to quit");
    attroff(COLOR_PAIR(1));

    refresh();
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
