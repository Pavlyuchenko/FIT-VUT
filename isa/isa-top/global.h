/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 *
 * Description: global structs and variables
 */

#ifndef GLOBAL_H
#define GLOBAL_H

// network headers
#include <pcap/pcap.h>

// pthread headers
#include <pthread.h>

// struct that holds all possible arguments passed to the program in the cli
typedef struct CLIArguments {
    char *interface;
    char sort;
    int interval;
    int cumulative;
} CLIArguments;

// context for the app, holds mutexes, packet capture and cli_args
typedef struct AppContext {
    pcap_t *packet_capture;
    pthread_mutex_t mutex;
    CLIArguments cli_args;
} AppContext;

extern AppContext app_context;

#endif // GLOBAL_H
