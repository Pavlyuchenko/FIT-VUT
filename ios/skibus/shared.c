#include "shared.h"

int map_shared_memory(SharedVars *shared)
{
    // shared semaphores
    shared->write_sync = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // int shared variables
    shared->A = mmap(NULL, sizeof(*(shared->A)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->skiers = mmap(NULL, sizeof(*(shared->skiers)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // shared output file
    shared->output = mmap(NULL, sizeof(FILE), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    return 0;
}

int unmap_shared_memory(SharedVars *shared)
{
    munmap(shared->write_sync, sizeof(sem_t));

    munmap(shared->A, sizeof(int));
    munmap(shared->skiers, sizeof(int));

    munmap(shared->output, sizeof(FILE));

    return 0;
}