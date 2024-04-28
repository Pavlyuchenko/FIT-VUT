/**
 * FIT VUT IOS Project 2
 * Name: Synchronization Skibus, inspired by Allen B. Downey: The Little Book of Semaphores (The Senate Bus problem)
 * Description: Implementation of a solution to the synchronization problem of the Skibus.
 * Author: Michal Pavlíček xpavlim00
 */

#include "proj2.h"

SharedVars shared;

void signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGKILL)
    {
        destroy_semaphores(&shared);
        unmap_shared_memory(&shared);
        fclose(output);

        exit(0);
    }
}

int main(int argc, char *argv[])
{
    parse_arguments(argc, argv);

    output = fopen(OUTPUT_FILE, "w");

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(output, NULL, _IONBF, 0);

    map_shared_memory(&shared);
    create_semaphores(&shared);

    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);

    *shared.A = 0;
    *shared.num_skiers_onboard = 0;

    // craete shared->waiting as an array of pointers to int
    *shared.waiting = 0;

    // create skibus process
    pid_t skibus_pid = fork();
    if (skibus_pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (skibus_pid == 0)
    {
        skibus_process(&shared, cli_args);

        exit(0);
    }

    // create <args.num_skiers> child processes
    for (int i = 1; i <= cli_args.num_skiers; i++)
    {
        pid_t child_pid = fork();

        if (child_pid == -1)
        {
            perror("fork");
            exit(1);
        }
        else if (child_pid == 0)
        {
            skier_process(&shared, cli_args, i);

            exit(0);
        }
    }

    // wait for all child processes to finish
    for (int i = 0; i < cli_args.num_skiers + 1; i++)
    {
        wait(NULL);
    }

    destroy_semaphores(&shared);
    unmap_shared_memory(&shared);

    return 0;
}
