/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 */

#include "stats.h"
#include "global.h"

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
    c_i->packets_sent_Tx = 0;
    c_i->packets_sent_Rx = 0;

    return c_i;
}

void insert_communication(CommunicationInfo *c_info) {
    Node *node = get_node(c_info);

    if (node->data == NULL) {
        node->data = c_info;
        node->data->packets_sent_Tx = 1;
        return;
    }

    add_communications(node->data, c_info);

    update_node_position(node);
}

CommunicationInfo *add_communications(CommunicationInfo *first,
                                      CommunicationInfo *second) {
    if (strcmp(first->src_ip, second->src_ip) == 0 &&
        first->src_port == second->src_port) {
        first->Tx += second->Tx;
        first->Rx += second->Rx;

        first->packets_sent_Rx += second->packets_sent_Rx;
        first->packets_sent_Tx += second->packets_sent_Tx;

        // TODO: free() second
        return first;
    }

    first->Tx += second->Rx;
    first->Rx += second->Tx;
    first->packets_sent_Rx += second->packets_sent_Tx;
    first->packets_sent_Tx += second->packets_sent_Rx;
    return first;
}

void *display_stats(void *arg) {
    CLIArguments *arguments = (CLIArguments *)arg;

	app_context.seconds_passed = 1;

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide the cursor

    // Initialize colors
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    while (1) {
        pthread_mutex_lock(&app_context.mutex);
        print_llist(MAX_DISPLAY_ROWS);
        pthread_mutex_unlock(&app_context.mutex);

        timeout(arguments->interval * 1000);
        if (arguments->cumulative == 0) {
            init_llist();
        }
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
		app_context.seconds_passed++;
    }

    // Clean up ncurses
    endwin();
    quit_app(1);

    return NULL;
}
