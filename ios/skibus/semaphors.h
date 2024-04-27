#ifndef SEMAPHORS_H
#define SEMAPHORS_H

#include <stdio.h>
#include <stdlib.h>

#include <semaphore.h> // sem_t, sem_init, sem_wait, sem_post, sem_destroy

// created based on "Downey, Allen B. Think OS: A Brief Introduction to Operating Systems, Chapter 11. Green Tea Press, 2014.
// Available at: http://greenteapress.com/thinkos/thinkos.pdf"
typedef sem_t Semaphore;

Semaphore *create_semaphore(int val);
void destroy_semaphore(Semaphore *sem);
void semaphore_wait(Semaphore *sem);
void semaphore_signal(Semaphore *sem);

#endif // SEMAPHORS_H