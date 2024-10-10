#ifndef STATS_H
#define STATS_H

#include <stdint.h>

#include "cli.h"
#include "linked_list.h"

#define MAX_COMMUNICATIONS 10000

CommunicationInfo *init_communication();

void insert_communication(CommunicationInfo *c_info);

CommunicationInfo *add_communications(CommunicationInfo *first,
                                      CommunicationInfo *second);
void *display_stats();

#endif // STATS_H
