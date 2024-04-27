/**
 * FIT VUT IOS Project 2
 * Name: Synchronization Skibus, inspired by Allen B. Downey: The Little Book of Semaphores (The Senate Bus problem)
 * Description: Implementation of a solution to the synchronization problem of the Skibus.
 * Author: Michal Pavlíček xpavlim00
 */

#include "proj2.h"

void shared_memory(SharedVars *shared)
{
    shared->mutex = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    shared->multiplex = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    shared->skibus = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    shared->all_aboard = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    shared->write_sync = mmap(NULL, sizeof(Semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    shared->A = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->skiers = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void skibus_process(SharedVars *shared, CLIArguments args)
{
    semaphore_wait(shared->write_sync);
    printf(BUS_STARTED, ++(*shared->A));
    semaphore_signal(shared->write_sync);
    /*
        // Riding to a bus stop
        srand(time(NULL));
        for (int i = 0; i < args.num_stops; i++)
        {
            usleep(rand() % args.tb);

            semaphore_wait(shared->write_sync);
            printf(BUS_ARRIVED, ++(*shared->A), i + 1);
            semaphore_signal(shared->write_sync);
        }*/
}

void skier_process(SharedVars *shared, Semaphore sem, int skier_id, CLIArguments args)
{
    int seed = time(NULL) + skier_id;
    srand(seed);
    // Generate random number from 1 to args.num_stop
    int stop = rand() % args.num_stops + 1;

    // Write into file/stdin
    sem_wait(&sem);
    fprintf(output, SKIER_STARTED, ++(*log_id), skier_id);
    fflush(output);
    sem_post(&sem);

    // Eating breakfast (time to get to the bus stop)
    /*usleep(rand() % args.tl);

    semaphore_wait(shared->write_sync);
    printf(SKIER_ARRIVED, ++(*shared->A), skier_id, stop);
    semaphore_signal(shared->write_sync);*/
}

int main(int argc, char *argv[])
{
    log_id_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    log_id = (int *)shmat(log_id_id, NULL, 0);

    SharedVars shared;
    CLIArguments args = parse_arguments(argc, argv);

    output = fopen("proj2.out", "w");

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(output, NULL, _IONBF, 0);

    // initialize shared variables
    shared.A = NULL;
    shared.skiers = NULL;

    shared_memory(&shared);

    // initialize semaphores
    shared.mutex = create_semaphore(1);
    shared.multiplex = create_semaphore(args.capacity);
    shared.skibus = create_semaphore(0);
    shared.all_aboard = create_semaphore(0);
    shared.write_sync = create_semaphore(1);

    *shared.A = 0;
    *shared.skiers = 0;

    // create skibus process
    pid_t skibus_pid = fork();

    if (skibus_pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (skibus_pid == 0)
    {
        // skibus_process(&shared, args);

        exit(0);
    }

    Semaphore sem;
    sem_init(&sem, 0, 1);
    // create <args.num_skiers> child processes
    for (int i = 1; i <= args.num_skiers; i++)
    {
        pid_t child_pid = fork();

        if (child_pid == -1)
        {
            perror("fork");
            exit(1);
        }
        else if (child_pid == 0)
        {
            skier_process(&shared, sem, i, args);

            exit(0);
        }
    }

    sem_destroy(shared.mutex);
    sem_destroy(shared.multiplex);
    sem_destroy(shared.skibus);
    sem_destroy(shared.all_aboard);

    // wait for all child processes to finish
    for (int i = 0; i < args.num_skiers + 1; i++)
    {
        wait(NULL);
    }

    sem_destroy(shared.write_sync);

    // free shared memory
    munmap(shared.mutex, sizeof(Semaphore));
    munmap(shared.multiplex, sizeof(Semaphore));
    munmap(shared.skibus, sizeof(Semaphore));
    munmap(shared.all_aboard, sizeof(Semaphore));
    munmap(shared.write_sync, sizeof(Semaphore));

    munmap(shared.A, sizeof(int));
    munmap(shared.skiers, sizeof(int));

    return 0;
}
