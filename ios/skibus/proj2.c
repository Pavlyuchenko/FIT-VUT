/**
 * FIT VUT IOS Project 2
 * Name: Synchronization Skibus, inspired by Allen B. Downey: The Little Book of Semaphores (The Senate Bus problem)
 * Description: Implementation of a solution to the synchronization problem of the Skibus.
 * Author: Michal Pavlíček xpavlim00
 */

#include "proj2.h"

int main(int argc, char *argv[])
{
    SharedVars shared;
    parse_arguments(argc, argv);

    output = fopen(OUTPUT_FILE, "w");

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(output, NULL, _IONBF, 0);

    map_shared_memory(&shared);
    create_semaphores(&shared);

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
