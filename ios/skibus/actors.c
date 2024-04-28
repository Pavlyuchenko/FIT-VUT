#include "actors.h"

void skibus_process(SharedVars *shared, CLIArguments args)
{
    // writes BUS STARTED
    sem_wait(shared->write_sync);
    fprintf(output, BUS_STARTED, ++(*shared->A));
    sem_post(shared->write_sync);

    // while goes until all skier got to the slope
    while (1)
    {
        // random seed
        srand(time(NULL));
        // for each bus stop
        for (int bus_stop = 1; bus_stop <= args.num_stops; bus_stop++)
        {
            // time between bus stops
            usleep(rand() % args.tb);

            // write bus arrived to bus stop
            lock_sem(shared->write_sync);
            fprintf(output, BUS_ARRIVED, ++(*shared->A), bus_stop);
            unlock_sem(shared->write_sync);

            // let skiers on board
            // I want to unlock mutex asap, cause it is used by skier right after.
            // that's why the if - else
            lock_sem(shared->mutex);
            if (*shared->waiting[bus_stop] > 0)
            {
                unlock_sem(shared->mutex);

                // if capacity not filled
                if (*shared->num_skiers_onboard < args.capacity)
                {
                    // let one skier in
                    unlock_sem(shared->bus_stops[bus_stop]);
                    // wait for all to board
                    lock_sem(shared->all_aboard);
                }
            }
            else
            {
                unlock_sem(shared->mutex);
            }

            // write departing from bus stop
            lock_sem(shared->write_sync);
            fprintf(output, BUS_LEAVING, ++(*shared->A), bus_stop);
            unlock_sem(shared->write_sync);
        }

        // ride to final
        usleep(rand() % args.tb);

        // write bus got to final
        lock_sem(shared->write_sync);
        fprintf(output, BUS_FINAL, ++(*shared->A));
        unlock_sem(shared->write_sync);

        // let skiers off
        unlock_sem(shared->let_skiers_leave);
        // wait for all to leave
        lock_sem(shared->all_aboard);

        // write leaving final
        lock_sem(shared->write_sync);
        fprintf(output, BUS_LEAVE_FINAL, ++(*shared->A));
        unlock_sem(shared->write_sync);

        // either end program or another round
        if (*shared->num_skiers_skiing >= args.num_skiers)
        {
            break;
        }
    }

    lock_sem(shared->write_sync);
    fprintf(output, BUS_FINISH, ++(*shared->A));
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
    // assign bus stop to a skier randomly
    int bus_stop = rand() % args.num_stops + 1;

    // Eating breakfast (time to get to the bus stop)
    usleep(rand() % args.tl);

    // write that skier got to the bus stop
    lock_sem(shared->write_sync);
    fprintf(output, SKIER_ARRIVED, ++(*shared->A), skier_id, bus_stop);
    unlock_sem(shared->write_sync);

    // increment number of skiers waiting at the bus stop
    lock_sem(shared->mutex);
    (*shared->waiting[bus_stop])++;
    unlock_sem(shared->mutex);

    // wait for the bus to arrive
    lock_sem(shared->bus_stops[bus_stop]);

    // once unlocked, update the variables
    ++(*shared->num_skiers_onboard);
    ++(*shared->num_skiers_skiing);

    // write that skier boarded
    lock_sem(shared->write_sync);
    fprintf(output, SKIER_BOARDING, ++(*shared->A), skier_id);
    unlock_sem(shared->write_sync);

    // if no skiers are left || capacity is filled: let bus know to leave
    // else let another skier in
    if (--(*shared->waiting[bus_stop]) == 0 || *shared->num_skiers_onboard >= args.capacity)
    {
        unlock_sem(shared->all_aboard);
    }
    else
    {
        unlock_sem(shared->bus_stops[bus_stop]);
    }

    // wait for the bus to arrive at the final stop
    lock_sem(shared->let_skiers_leave);
    --(*shared->num_skiers_onboard);

    lock_sem(shared->write_sync);
    fprintf(output, SKIER_GOING_TO_SKI, ++(*shared->A), skier_id);
    unlock_sem(shared->write_sync);

    if (*shared->num_skiers_onboard == 0)
    {
        unlock_sem(shared->all_aboard);
    }
    else
    {
        unlock_sem(shared->let_skiers_leave);
    }
}