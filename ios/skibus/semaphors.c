#include "semaphors.h"

Semaphore *create_semaphore(int val)
{
    Semaphore *sem = (Semaphore *)malloc(sizeof(Semaphore));
    int ret_code = sem_init(sem, 1, val);

    if (ret_code == -1)
    {
        perror("sem_init");
        exit(1);
    }

    return sem;
}

void destroy_semaphore(Semaphore *sem)
{
    int ret_code = sem_destroy(sem);
    if (ret_code == -1)
    {
        perror("sem_destroy");
        exit(1);
    }

    free(sem);
}

void semaphore_wait(Semaphore *sem)
{
    int ret_code = sem_wait(sem);

    if (ret_code != 0)
    {
        perror("sem_wait");
        exit(1);
    }
}

void semaphore_signal(Semaphore *sem)
{
    int ret_code = sem_post(sem);

    if (ret_code != 0)
    {
        perror("sem_post");
        exit(1);
    }
}
