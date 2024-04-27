#ifndef PROJ2_H
#define PROJ2_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h> // pid_t
#include <unistd.h>    // fork, getpid
#include <sys/mman.h>  // mmap
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "semaphors.h" // creating, deleting, waiting, signaling semaphores
#include "actors.h"    // code for skibus and skiers
#include "cli.h"

#endif // PROJ2_H