#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdlib.h>

typedef struct CLIArguments
{
    int num_skiers;
    int num_stops;
    int capacity; // of the bus
    int tl;       // max time that skier waits before coming to the bus stop
    int tb;       // max time of bus ride between stops
} CLIArguments;

void parse_arguments(int argc, char *argv[]);

CLIArguments cli_args;

#endif // CLI_H