#include "semaphors.h"

int *create_semaphores(SharedVars *shared)
{
    shared->write_sync = sem_open("/xpavlim00.ios.proj2.write_sync", O_CREAT | O_EXCL, 0666, 1);

    // create cli_args->num_stops semaphores for bus stops
    shared->bus_stops = malloc(cli_args.num_stops * sizeof(Semaphore));
    for (int i = 0; i < cli_args.num_stops; i++)
    {
        shared->bus_stops[i] = sem_open("/xpavlim00.ios.proj2.bus_stop" + i, O_CREAT | O_EXCL, 0666, 0);
    }

    return 0; // success
}

int destroy_semaphores(SharedVars *shared)
{
    sem_close(shared->write_sync);
    sem_unlink("/xpavlim00.ios.proj2.write_sync");

    // close bus_stops
    for (int i = 0; i < cli_args.num_stops; i++)
    {
        sem_close(shared->bus_stops[i]);
        sem_unlink("/xpavlim00.ios.proj2.bus_stop" + i);
    }

    return 0;
}

int lock_sem(Semaphore *sem)
{
    int ret_code = sem_wait(sem);

    if (ret_code != 0)
    {
        perror("sem_wait");
        exit(1);
    }

    return 0;
}

int unlock_sem(Semaphore *sem)
{
    int ret_code = sem_post(sem);

    if (ret_code != 0)
    {
        perror("sem_post");
        exit(1);
    }

    return 0;
}
