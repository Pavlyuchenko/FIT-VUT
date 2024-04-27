#include "cli.h"

CLIArguments parse_arguments(int argc, char *argv[])
{
    if (argc != 6)
    {
        fprintf(stderr, "Usage: %s <num_skiers> <num_stops> <capacity> <tl> <tb>\n", argv[0]);
        exit(1);
    }

    // check that all 6 arguments are integers
    for (int i = 1; i < argc; i++)
    {
        for (int j = 0; argv[i][j] != '\0'; j++)
        {
            if (argv[i][j] < '0' || argv[i][j] > '9')
            {
                fprintf(stderr, "All arguments must be integers\n");
                exit(1);
            }
        }
    }

    CLIArguments args;
    args.num_skiers = atoi(argv[1]);
    if (args.num_skiers < 0 || args.num_skiers >= 20000)
    {
        fprintf(stderr, "Error: (1st argument) Number of skiers must be in range 0 < num_skiers < 20000\n");
        exit(1);
    }

    args.num_stops = atoi(argv[2]);
    if (args.num_stops <= 0 || args.num_stops > 10)
    {
        fprintf(stderr, "Error: (2nd argument) Number of stops must be in range 0 < num_stops <= 10\n");
        exit(1);
    }

    args.capacity = atoi(argv[3]);
    if (args.capacity < 10 || args.capacity > 100)
    {
        fprintf(stderr, "Error: (3rd argument) Capacity of the bus must be in range 10 <= capacity <= 100\n");
        exit(1);
    }

    args.tl = atoi(argv[4]);
    if (args.tl < 0 || args.tl > 10000)
    {
        fprintf(stderr, "Error: (4th argument) Max time that skier waits for bus must be in range 0 < tl <= 10000\n");
        exit(1);
    }

    args.tb = atoi(argv[5]);
    if (args.tb < 0 || args.tb > 1000)
    {
        fprintf(stderr, "Error: (5th argument) Max time of bus ride between stops must be in range 0 <= tb <= 1000\n");
        exit(1);
    }

    return args;
}