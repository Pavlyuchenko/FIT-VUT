/**
 * isa-top : Application gathering statistics about network traffic
 * @Description: isa-top is an application that listens on specified interface
 *				 and outputs network traffic statistics. Written
 *				 for course ISA @ FIT VUT, Brno.
 * @Author: Michal Pavlíček xpavlim00
 * @Note: Some starter code was taken from my IPK Project 2: A packet sniffer.
 */

#include "isa-top.h"

AppContext app_context = {0};

int main(int argc, char *argv[]) {
	/*CommunicationInfo *ci = init_communication();
	ci->src_ip = "192";
	ci->dst_ip = "100";
	ci->protocol = "";
	ci->packets_sent++;

	ci->Rx = 100;
	ci->Tx = 10;
	add_communication(ci);

	CommunicationInfo *ci2 = init_communication();
	ci2->src_ip = "191";
	ci2->dst_ip = "100";
	ci2->protocol = "";
	ci2->packets_sent++;

	ci2->Rx = 1;
	ci2->Tx = 2;
	add_communication(ci2);
	*/

    CLIArguments arguments = parse_arguments(argc, argv);

	init_llist();

    pthread_t capture_thread, stats_thread;
    pthread_mutex_init(&app_context.mutex, NULL);

    if (pthread_create(&capture_thread, NULL, capture_packets,
                       (void *)&arguments) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_create./n");
        exit(1);
    }
    if (pthread_create(&stats_thread, NULL, display_stats,
                       (void *)&arguments) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_create./n");
        exit(1);
    }

    if (pthread_join(capture_thread, NULL) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_join./n");
        exit(1);
    }
    if (pthread_join(stats_thread, NULL) != 0) {
        fprintf(stderr, "ERROR: error while running pthread_join./n");
        exit(1);
    }

    return 0;
}
