#include "actors.h"

void skibus_process(SharedVars *shared, CLIArguments args)
{
    sem_wait(shared->write_sync);
    fprintf(output, BUS_STARTED, ++(*shared->A));
    sem_post(shared->write_sync);

    // Riding to a bus stop
    srand(time(NULL));
    for (int i = 0; i < args.num_stops; i++)
    {
        usleep(rand() % args.tb);

        lock_sem(shared->write_sync);
        fprintf(output, BUS_ARRIVED, ++(*shared->A), i + 1);
        unlock_sem(shared->write_sync);

        // TODO: let passengers board

        lock_sem(shared->write_sync);
        fprintf(output, BUS_LEAVING, ++(*shared->A), i + 1);
        unlock_sem(shared->write_sync);
    }

    // ride to final
    usleep(rand() % args.tb);

    lock_sem(shared->write_sync);
    fprintf(output, BUS_FINAL, ++(*shared->A));
    unlock_sem(shared->write_sync);
}

void skier_process(SharedVars *shared, CLIArguments args, int skier_id)
{
    // write that skier started
    sem_wait(shared->write_sync);
    fprintf(output, SKIER_STARTED, ++(*shared->A), skier_id);
    sem_post(shared->write_sync);

    // randomness
    int seed = time(NULL) * skier_id;
    srand(seed);
    int bus_stop = rand() % args.num_stops + 1;

    // Eating breakfast (time to get to the bus stop)
    usleep(rand() % args.tl);

    lock_sem(shared->write_sync);
    fprintf(output, SKIER_ARRIVED, ++(*shared->A), skier_id, bus_stop);
    unlock_sem(shared->write_sync);

    // TODO: Figure out how to signal bus stops than another passenger arrived
}