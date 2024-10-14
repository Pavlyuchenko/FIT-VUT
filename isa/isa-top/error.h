#ifndef ERROR_H
#define ERROR_H

#include <pcap/pcap.h>
#include <stdlib.h>

#include "global.h"

extern char errbuff[PCAP_ERRBUF_SIZE];

void quit_app(int signal);

typedef enum {
    ERR_MALLOC = 1,
	ERR_THREAD,
	ERR_CLI,
	ERR_PACKET_CAPTURE,
} ErrorType;

void throw_error(const char *message, ErrorType err_type);

#endif // ERROR_H
