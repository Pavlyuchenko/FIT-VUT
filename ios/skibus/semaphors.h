/**
 * @file semaphors.h
 * @brief File containing semaphores operations.
 * @author Michal Pavlíček xpavlim00
 */

#ifndef SEMAPHORS_H
#define SEMAPHORS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <semaphore.h> // sem_t, sem_init, sem_wait, sem_post, sem_destroy

#include "shared.h"
#include "cli.h"

int create_semaphores(SharedVars *shared);
int destroy_semaphores(SharedVars *shared);
int lock_sem(Semaphore *sem);
int unlock_sem(Semaphore *sem);

#endif // SEMAPHORS_H