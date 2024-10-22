/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 *
 * Description: File for statistics gathering and displaying.
 */

#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include <string.h>

#include "cli.h"
#include "error.h"
#include "linked_list.h"
#define MAX_COMMUNICATIONS 100000

// Initializes CommunicationInfo structure
CommunicationInfo *init_communication();

// Inserts CommunicationInfo into linked list structure
void insert_communication(CommunicationInfo *c_info);

// Adds two CommunicationInfo structs. Adds Rx, Tx and packets_sent
CommunicationInfo *add_communications(CommunicationInfo *first,
                                      CommunicationInfo *second);

// Main function for stats display, used by threads
void *display_stats(void *arg);

#endif // STATS_H
