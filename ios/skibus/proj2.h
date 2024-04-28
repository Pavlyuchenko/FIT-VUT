/**
 * @file proj2.h
 * @brief Main file for the skibus problem
 * @author Michal Pavlíček xpavlim00
 */

#ifndef PROJ2_H
#define PROJ2_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h> // pid_t
#include <unistd.h>    // fork, getpid
#include <sys/mman.h>  // mmap
#include <sys/wait.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <signal.h>

#include "cli.h"
#include "shared.h"
#include "semaphors.h"
#include "actors.h"

#define OUTPUT_FILE "proj2.out"

void signal_handler(int signum);

#endif // PROJ2_H