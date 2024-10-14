#ifndef GLOBAL_H
#define GLOBAL_H

// network headers
#include <pcap/pcap.h>

// pthread headers
#include <pthread.h>

typedef struct CLIArguments
{
    char *interface;
	char sort;
	int interval;
	int cumulative;
} CLIArguments;


typedef struct AppContext {
	pcap_t *packet_capture;
	pthread_mutex_t mutex;
	CLIArguments cli_args;
} AppContext;

extern AppContext app_context;

#endif // GLOBAL_H
