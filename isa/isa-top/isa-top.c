/**
 * isa-top : Application gathering statistics about network traffic
 * @Description: isa-top is an application that listens on specified interface
 *				 and outputs network traffic statistics. Written
 *				 for course ISA @ FIT VUT, Brno.
 * @Author: Michal Pavlíček xpavlim00
 * @Note: Some starter code was taken from my IPK Project 2: A packet sniffer.
 */

#include "isa-top.h"
#include "error.h"

AppContext app_context = {0};

int main(int argc, char *argv[]) {
	app_context.cli_args =  parse_arguments(argc, argv);

    init_llist();

    pthread_t capture_thread, stats_thread;
    pthread_mutex_init(&app_context.mutex, NULL);

    if (pthread_create(&capture_thread, NULL, capture_packets,
                       (void *)&app_context.cli_args) != 0) {
		throw_error("pthread_create", ERR_THREAD);
    }
    if (pthread_create(&stats_thread, NULL, display_stats,
                       (void *)&app_context.cli_args) != 0) {
		throw_error("pthread_create", ERR_THREAD);
    }

    if (pthread_join(capture_thread, NULL) != 0) {
		throw_error("pthread_join", ERR_THREAD);
    }
    if (pthread_join(stats_thread, NULL) != 0) {
		throw_error("pthread_join", ERR_THREAD);
    }

    return 0;
}
