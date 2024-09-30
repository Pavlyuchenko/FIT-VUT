#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int opt;
	char *interface = NULL;	// which interface is listened on
	char sort = 'b';		// either b or p meaning bytes/packets per second 
	int interval = 1;		// how often are stats updated

	// argument parsing done with help from:
	// https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html 
	while ((opt = getopt(argc, argv, "i:s:t:")) != -1) {
		switch (opt) {
			case 'i':
				interface = optarg;
				break;
			case 's':
				sort = optarg[0];

				if (!(sort == 'b' || sort == 'p')) {
					fprintf(stderr, "ERROR: Only valid options for -s are b or p.");
				}
				break;
			case 't':
				char *endptr;
				long temp_interval = strtol(optarg, &endptr, 10);

				if (*endptr != '\0' || temp_interval <= 0) {
					fprintf(stderr, "ERROR: Invalid interval '%s'. Must be a positive integer.\n", optarg);
					exit(1);
				}
				interval = (int)temp_interval;
				break;
			case '?':
				if (optopt == 'i' || optopt == 's' || optopt == 't') {
					fprintf(stderr, "ERROR: Option -%c requires an argument.\n", optopt);
					exit(1);
				} else if (isprint(optopt)) {
					fprintf(stderr, "ERROR: Unknown option -%c.\n", optopt);
					exit(1);
				} else {
					fprintf(stderr, "ERROR: Unknown character.\n");
					exit(1);
				}
				break;
		}
	}
	printf("%s, %c, %i\n", interface, sort, interval);
}
