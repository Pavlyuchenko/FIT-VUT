#ifndef EPOLL_HANDLER_H
#define EPOLL_HANDLER_H

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <termios.h>
#include <sys/time.h>

#include "cli.h"
#include "tcp_handler.h"
#include "fsm_udp.h"

#define MAX_EVENTS 10

int get_epoll_fd();

int get_sock_fd(CLIArguments args, int epoll_fd);

#endif // EPOLL_HANDLER_H