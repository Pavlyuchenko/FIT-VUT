#ifndef FSM_TCP_H
#define FSM_TCP_H

#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include "cli.h"
#include "tcp_handler.h"

typedef enum
{
    START,
    AUTH,
    OPEN,
    ERROR,
    END
} FSMState;

void fsm_tcp(int i, struct epoll_event events[], int sock_fd);

#endif