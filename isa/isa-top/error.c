#include "error.h"

char errbuff[PCAP_ERRBUF_SIZE];

void close_all() {
    pcap_close(app_context.packet_capture);
    return;
}

void quit_app(int signal) {
    close_all();

    printf("\nExiting due to signal %d...\n", signal);
    exit(0);
}

void throw_error(const char *message, ErrorType err_type) {
    switch (err_type) {
    case ERR_MALLOC:
        fprintf(stderr, "ERROR: Malloc failed in %s.\n", message);
        break;
    }

	close_all();

    exit(err_type);
}
