/**
 * @file shared.h
 * @brief File containing shared memory operations.
 * @author Michal Pavlíček xpavlim00
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>

#include <semaphore.h> // sem_t, sem_init, sem_wait, sem_post, sem_destroy
#include <sys/mman.h>  // mmap, munmap

#include "cli.h"

// created based on "Downey, Allen B. Think OS: A Brief Introduction to Operating Systems, Chapter 11. Green Tea Press, 2014.
// Available at: http://greenteapress.com/thinkos/thinkos.pdf"
typedef sem_t Semaphore;

typedef struct SharedVars
{
    // as per The Little Book of Semaphores 7.4.1
    Semaphore *mutex;
    Semaphore *write_sync;
    Semaphore *bus_capacity;
    Semaphore *all_aboard;
    Semaphore *let_skiers_leave;

    // array of pointers to integer
    Semaphore **bus_stops;

    // pointers to int
    int *A;
    int *num_skiers_onboard;
    int *num_skiers_skiing;
    int *waiting[11];

    FILE *output;
} SharedVars;

FILE *output;

int map_shared_memory(SharedVars *shared);
int unmap_shared_memory(SharedVars *shared);

#endif // SHARED_H