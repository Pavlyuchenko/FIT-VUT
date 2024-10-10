#ifndef GLOBAL_H
#define GLOBAL_H

// network headers
#include <pcap/pcap.h>

// pthread headers
#include <pthread.h>

typedef struct AppContext {
	pcap_t *packet_capture;
	pthread_mutex_t mutex; // PTHREAD_MUTEX_INITIALIZER;
} AppContext;

extern AppContext app_context;

#endif // GLOBAL_H
