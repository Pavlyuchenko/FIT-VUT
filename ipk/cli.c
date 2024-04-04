#include "cli.h"
#include "tcp_handler.h"

char DISPLAYNAME[256];

CLIArguments get_cli_arguments(int argc, char *argv[])
{
    CLIArguments args;
    args.transport_protocol = -1;
    args.server_ip = NULL;
    args.server_port = DEFAULT_SERVER_PORT;
    args.udp_timeout = DEFAULT_UDP_TIMEOUT;
    args.udp_retransmissions = 3;

    int opt;
    while ((opt = getopt(argc, argv, "t:s:p:d:r:h")) != -1)
    {
        switch (opt)
        {
        case 't':
            if (strcasecmp(optarg, "tcp") == 0)
            {
                args.transport_protocol = TCP;
            }
            else if (strcasecmp(optarg, "udp") == 0)
            {
                args.transport_protocol = UDP;
            }
            break;
        case 's':
            args.server_ip = optarg;
            break;
        case 'p':
            args.server_port = atoi(optarg);
            break;
        case 'd':
            args.udp_timeout = atoi(optarg);
            break;
        case 'r':
            args.udp_retransmissions = atoi(optarg);
            break;
        case 'h':
            printf("Usage: %s -t <tcp|udp> -s <server_ip> -p <server_port> -d <udp_timeout> -r <udp_retransmissions> -h\n", argv[0]);
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Usage: %s -t <tcp|udp> -s <server_ip> -p <server_port> -d <udp_timeout> -r <udp_retransmissions> -h\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (!(args.transport_protocol == TCP || args.transport_protocol == UDP) || args.server_ip == NULL)
    {
        fprintf(stderr, "Usage: %s -t <tcp|udp> -s <server_ip> -p <server_port> -d <udp_timeout> -r <udp_retransmissions> -h\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (args.server_port < 1024)
    {
        fprintf(stderr, "Invalid server port. Must be in range 1024 - 65535.\n");
        exit(EXIT_FAILURE);
    }

    return args;
}

UserCommand parse_command(char *command)
{
    if (strncmp(command, "/auth", 5) == 0)
    {
        return AUTHENTICATE;
    }
    else if (strncmp(command, "/join", 5) == 0)
    {
        return JOIN;
    }
    else if (strncmp(command, "/rename", 7) == 0)
    {
        return RENAME;
    }
    else if (strncmp(command, "/help", 5) == 0)
    {
        return HELP;
    }
    else if (command[0] == '/')
    {
        return UNKNOWN;
    }
    else
    {
        return MESSAGE;
    }
}

void show_help()
{
    printf("Available commands:\n");
    printf("\t/auth <username> <password> <display_name> - authenticate user\n");
    printf("\t/join <channel_id> - join a chat channel\n");
    printf("\t/rename <new_display_name> - change display name\n");
    printf("\t/help - print this help message\n");
    printf("\t<message> - send a message to the current chat channel\n");
}

AuthResponse *processAuthCmd(char *buffer)
{
    AuthResponse *response = malloc(sizeof(AuthResponse));
    strtok(buffer, " "); // skip the /auth command
    response->username = strtok(NULL, " ");
    response->password = strtok(NULL, " ");
    response->display_name = strtok(NULL, "\0");

    // Check if display_name is NULL or contains only newline characters
    if (response->display_name != NULL && strcspn(response->display_name, "\n") != 0)
    {
        response->display_name[strcspn(response->display_name, "\n")] = 0;
    }

    return response;
}