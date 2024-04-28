#ifndef ACTORS_H
#define ACTORS_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "semaphors.h"
#include "cli.h"

// print string placeholders
#define SKIER_STARTED "%d: L %d: started\n"
#define SKIER_ARRIVED "%d: L %d: arrived to %d\n"
#define SKIER_BOARDING "%d: boarding\n"
#define SKIER_GOING_TO_SKI "%d: going to ski\n"
#define BUS_STARTED "%d: BUS: started\n"
#define BUS_ARRIVED "%d: BUS: arrived to %d\n"
#define BUS_LEAVING "%d: BUS: leaving %d\n"
#define BUS_FINAL "%d: BUS: arrived to final\n"
#define BUS_LEAVE_FINAL "%d: BUS: leaving final\n"
#define BUS_FINISH "%d: BUS: finish\n"

void skibus_process(SharedVars *shared, CLIArguments args);

void skier_process(SharedVars *shared, CLIArguments args, int skier_id);

#endif // ACTORS_H