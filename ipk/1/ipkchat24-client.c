/**
 * IPK24 Chat Server
 * @brief Implementation of the IPK24-CHAT protocol for a client app that communicates with a remote server.
 * @author Michal Pavlíček xpavlim00
 */

/**
 * Usage:
 *    ipkchat24 -t {tcp|udp} -s {server_ip} -p [server_port] -d [udp_timeout] -r [udp_retransmissions] -h
 *
 * Arguments of the script:
 *  arg |   default value       | possible values       | explanation
 *  -t	    User provided	    tcp or udp	            Transport protocol used for connection
 *  -s	    User provided	    IP address or hostname	Server IP or hostname
 *  -p	    4567	            uint16	                Server port
 *  -d	    250	                uint16	                UDP confirmation timeout
 *  -r	    3	                uint8	                Maximum number of UDP retransmissions
 *  -h			                                        Prints program help output and exits
 *
 * Message types:
 *  - ERR (DisplayName, MessageContent) = indicates there has been an error while communicating with the other server
 *  - CONFIRM = only for UDP, confirms delivery to the other server
 *  - REPLY (bool, MessageContent) = contains info about a status of previous message
 *  - AUTH (Username, DisplayName, Password) = use for login, provide username, display name and password
 *  - JOIN (ChannelID, DisplayName) = request to join a chat (there is a default chat in a server)
 *  - MSG (DisplayName, MessageContent) = message to a channel
 *  - BYE = final message sent, terminates the communication
 *
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
// strncasecmp
#include <strings.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <assert.h>
#include <stdbool.h>
#include <signal.h>

// my headers
#include "cli.h"
#include "tcp_handler.h"
#include "fsm_tcp.h"
#include "fsm_udp.h"
#include "epoll_handler.h"

volatile sig_atomic_t exit_flag = 0;

void sigint_handler()
{
    exit_flag = 1;
    fprintf(stderr, "Exiting application...\n");
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    CLIArguments args = get_cli_arguments(argc, argv);

    struct epoll_event events[MAX_EVENTS];
    int epoll_fd, sock_fd, num_fds;

    epoll_fd = get_epoll_fd();
    sock_fd = get_sock_fd(args, epoll_fd); // either TCP or UDP

    while (!exit_flag)
    {
        num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < num_fds; i++)
        {
            if (events[i].data.fd == sock_fd)
            {
                if (events[i].events & EPOLLHUP)
                {
                    fprintf(stderr, "Server disconnected\n");
                    exit(0);
                }
            }

            if (args.transport_protocol == TCP)
            {
                fsm_tcp(i, events, sock_fd);
            }
            else if (args.transport_protocol == UDP)
            {
                fsm_udp(i, events, sock_fd, args);
            }
        }
    }
    if (args.transport_protocol == TCP)
    {
        // FIN flag
        send(sock_fd, "BYE\r\n", 5, 0);
        close(sock_fd);
    }
    else if (args.transport_protocol == UDP)
    {
        // BYE: 0xFF | MessageID
        char message[3];
        construct_bye_udp(message, message_id);

        udp_send(sock_fd, args, message, 3);
    }

    close(epoll_fd);

    return 0;
}