#include "cli.h"
#include "error.h"

CLIArguments parse_arguments(int argc, char *argv[]) {
    /*  ./ipk-sniffer
     *      [-i interface]
     *      {-s sort by}
     *      {-t interval for stat update}
     *      {-h show this help}
     */
    CLIArguments cli_args;
    int opt;
    cli_args.interface = NULL; // which interface is listened on
    cli_args.sort = 'b';       // either b or p meaning bytes/packets per second
    cli_args.interval = 1;     // how often stats are updated
	cli_args.cumulative = 0;

    // argument parsing done with help from:
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    while ((opt = getopt(argc, argv, "hi:s:t:c")) != -1) {
        long temp_interval;
        switch (opt) {
        case 'i':
            cli_args.interface = optarg;
            break;
        case 's':
            cli_args.sort = optarg[0];

            if (!(cli_args.sort == 'b' || cli_args.sort == 'p')) {
				throw_error(" for -s you have to use b or p", ERR_CLI);
            }
            break;
        case 't':
            char *endptr;
            temp_interval = strtol(optarg, &endptr, 10);

            if (*endptr != '\0' || temp_interval <= 0) {
				throw_error("-t must get positive integer", ERR_CLI);
            }
            cli_args.interval = (int)temp_interval;
            break;
		case 'c':
			cli_args.cumulative = 1;
			break;
        case 'h':
            printf("isa-top: displays network traffic on an interface by host. "
                   "Created for course ISA @ FIT VUT."
                   "\n\nSynopsis: "
                   "./ipk-sniffer\n\t[-i interface]\n\t{-s sort by b|p}\n\t{-t "
                   "update interval}\n\t{-h show this help}\n\t{-c cumulative stats}\n\n"
                   "\t-h\t\tdisplays this message\n\t-i\t\tdisplays list of "
                   "available "
                   "interfaces\n\t-i "
                   "interface\t\tlisten on this interfac\n\t-s "
                   "b|p\t\tspecifies the sorting of displayed statistics, "
                   "either by bytes or packets.\n\t-t interval\t\tsets the "
                   "update interval of statistics\n\t-c\t\tshow cumulative stats over whole duration of program\n\nisa-top, copyright "
                   "Michal Pavlíček <xpavlim00@stud.fit.vutbr.cz, "
                   "michaelg.pavlicek@gmail.com>, 2024.\n");
            exit(0);
            break;
        case '?':
            if (optopt == 'i') {
                pcap_if_t *devices = NULL;
                if (pcap_findalldevs(&devices, errbuff) != PCAP_ERROR) {
                    if (devices != NULL) {
                        pcap_if_t *curr;
                        for (curr = devices; curr; curr = curr->next) {
                            if ((curr->flags & PCAP_IF_UP) &&
                                (curr->flags & PCAP_IF_RUNNING)) {
                                printf("%s\n", curr->name);
                            }
                        }
                    } else {
                        fprintf(stderr, "No devices were found\n");
                    }

                    pcap_freealldevs(devices);
                } else {
                    fprintf(stderr, "Error: %s\n", errbuff);
                }

                exit(0);
            }

            if (optopt == 's' || optopt == 't') {
				throw_error("options require an argument", ERR_CLI);
            } else if (isprint(optopt)) {
				throw_error("unknown option", ERR_CLI);
            } else {
				throw_error("unknown character", ERR_CLI);
            }
            break;
        }
    }
    if (!cli_args.interface) {
		throw_error("no interface was specified. Try -h", ERR_CLI);
    }

    return cli_args;
}

