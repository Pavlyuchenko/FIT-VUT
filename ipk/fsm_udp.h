#ifndef FSM_UDP_H
#define FSM_UDP_H

#include "epoll_handler.h"
#include "fsm_tcp.h"
#include "cli.h"
#include "udp_handler.h"

extern unsigned char message_id[2];

typedef enum
{
    CONFIRM_UDP,
    REPLY_UDP,
    AUTH_UDP,
    JOIN_UDP,
    MSG_UDP,
    ERR_UDP = 254, // 0xFE
    BYE_UDP = 255  // 0xFF
} UDPMessageType;

void fsm_udp(int i, struct epoll_event events[], int sock_fd, CLIArguments args);

#endif // FSM_UDP_H