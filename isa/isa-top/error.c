/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 */

#include "error.h"
#include "global.h"
#include <ncurses.h>

char errbuff[PCAP_ERRBUF_SIZE];

// closes any running instances
void close_all() {
    if (app_context.packet_capture) {
        pcap_close(app_context.packet_capture);
    }

	endwin();

    return;
}

// called function upon recieving closing signal
void quit_app(int signal) {
    close_all();

    printf("\nExiting due to signal %d...\n", signal);
    exit(0);
}

// error throwing
void throw_error(const char *message, ErrorType err_type) {
    switch (err_type) {
    case ERR_MALLOC:
        fprintf(stderr, "ERROR: Malloc failed in %s.\n", message);
        break;
    case ERR_THREAD:
        fprintf(stderr, "ERROR: Threading failed in %s.\n", message);
        break;
    case ERR_CLI:
        fprintf(stderr, "ERROR: There has been an error with CLI %s.\n",
                message);
        break;
    case ERR_PACKET_CAPTURE:
        fprintf(stderr, "ERROR: There has been an error with capturing %s.\n",
                message);
        break;
    }

    close_all();

    exit(err_type);
}
