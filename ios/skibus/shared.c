#include "shared.h"

int map_shared_memory(SharedVars *shared)
{
    // shared semaphores
    shared->write_sync = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->mutex = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->bus_capacity = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->all_aboard = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // array of pointers to semaphores
    shared->bus_stops = mmap(NULL, sizeof(Semaphore *) * (cli_args.num_stops + 1), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < cli_args.num_stops + 1; i++)
    {
        shared->bus_stops[i] = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }

    // int shared variables
    shared->A = mmap(NULL, sizeof(*(shared->A)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->num_skiers_onboard = mmap(NULL, sizeof(*(shared->num_skiers_onboard)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->num_skiers_skiing = mmap(NULL, sizeof(*(shared->num_skiers_skiing)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // array of 10 int pointers
    for (int i = 0; i <= 10; i++)
    {
        shared->waiting[i] = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }

    // shared output file
    shared->output = mmap(NULL, sizeof(FILE), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    return 0;
}

int unmap_shared_memory(SharedVars *shared)
{
    munmap(shared->write_sync, sizeof(sem_t));
    munmap(shared->mutex, sizeof(sem_t));
    munmap(shared->bus_capacity, sizeof(sem_t));
    munmap(shared->all_aboard, sizeof(sem_t));

    for (int i = 0; i < cli_args.num_stops + 1; i++)
    {
        munmap(shared->bus_stops[i], sizeof(sem_t));
    }

    munmap(shared->bus_stops, sizeof(sem_t *) * cli_args.num_stops);

    munmap(shared->A, sizeof(int *));
    munmap(shared->num_skiers_onboard, sizeof(int *));
    munmap(shared->num_skiers_skiing, sizeof(int *));

    // array of 10 int pointers
    munmap(shared->waiting, sizeof(int *) * 11);

    munmap(shared->output, sizeof(FILE));

    return 0;
}