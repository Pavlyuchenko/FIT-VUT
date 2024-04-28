#include "semaphors.h"

int *create_semaphores(SharedVars *shared)
{
    shared->write_sync = sem_open("/xpavlim00.ios.proj2.write_sync", O_CREAT | O_EXCL, 0666, 1);
    shared->mutex = sem_open("/xpavlim00.ios.proj2.mutex", O_CREAT | O_EXCL, 0666, 1);
    shared->bus_capacity = sem_open("/xpavlim00.ios.proj2.bus_capacity", O_CREAT | O_EXCL, 0666, cli_args.capacity);
    shared->all_aboard = sem_open("/xpavlim00.ios.proj2.all_aboard", O_CREAT | O_EXCL, 0666, 0);
    shared->let_skiers_leave = sem_open("/xpavlim00.ios.proj2.let_skiers_leave", O_CREAT | O_EXCL, 0666, 0);

    // Create semaphores for bus stops
    shared->bus_stops = malloc((cli_args.num_stops + 1) * sizeof(sem_t *));
    for (int i = 0; i < cli_args.num_stops + 1; i++)
    {
        char semaphore_name[50];
        sprintf(semaphore_name, "/xpavlim00.ios.proj2.bus_stop%d", i);
        shared->bus_stops[i] = sem_open(semaphore_name, O_CREAT | O_EXCL, 0666, 0);
    }

    return 0; // success
}

int destroy_semaphores(SharedVars *shared)
{
    sem_close(shared->write_sync);
    sem_close(shared->mutex);
    sem_close(shared->bus_capacity);
    sem_close(shared->all_aboard);
    sem_close(shared->let_skiers_leave);

    sem_unlink("/xpavlim00.ios.proj2.write_sync");
    sem_unlink("/xpavlim00.ios.proj2.mutex");
    sem_unlink("/xpavlim00.ios.proj2.bus_capacity");
    sem_unlink("/xpavlim00.ios.proj2.all_aboard");
    sem_unlink("/xpavlim00.ios.proj2.let_skiers_leave");

    // Close and unlink bus_stops
    for (int i = 0; i < cli_args.num_stops + 1; i++)
    {
        char semaphore_name[50];
        sprintf(semaphore_name, "/xpavlim00.ios.proj2.bus_stop%d", i);
        sem_close(shared->bus_stops[i]);
        sem_unlink(semaphore_name);
    }

    free(shared->bus_stops);

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
