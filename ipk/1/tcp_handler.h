#ifndef SERVER_H
#define SERVER_H

#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stddef.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>     // for strtok
#include <unistd.h>     // for close
#include <arpa/inet.h>  // for inet_ntoa
#include <netdb.h>      // for getaddrinfo
#include <sys/socket.h> // for send
#include <netinet/in.h> // for sockaddr_in
#include <errno.h>      // for perror
#include <sys/types.h>  // for getaddrinfo
#include <sys/socket.h> // for getaddrinfo
#include <stdbool.h>    // for bool

// for DISPLAYNAME
#include "cli.h"

// Message blueprints for possible server responses
#define AUTH_MESSAGE_PLACEHOLDER "AUTH %s AS %s USING %s\r\n" // Username, DisplayName, Password
#define JOIN_MESSAGE_PLACEHOLDER "JOIN %s AS %s\r\n"          // ChannelID, DisplayName
#define MSG_PLACEHOLDER "MSG FROM %s IS %s\r\n"               // DisplayName, MessageContent
#define BYE_MESSAGE "BYE\r\n"

#define MAX_BUFFER_SIZE 1024

// what the server may respond with:
// REPLY {"OK"|"NOK"} IS {MessageContent}\r\n
typedef struct
{
    bool is_ok;
    char message_content[1400];
} ReplyResponse;
// ERR FROM {DisplayName} IS {MessageContent}\r\n
typedef struct
{
    char *display_name;
    char message_content[1400];
} ErrResponse;

// JOIN {ChannelID} AS {DisplayName}\r\n
typedef struct
{
    char *channel_id;
    char *display_name;
} JoinResponse;
// MSG FROM {DisplayName} IS {MessageContent}\r\n
typedef struct
{
    char *display_name;
    char message_content[1400];
} MsgResponse;

int authenticate(char *buffer, int sock_fd);

char *get_ip(char *server_ip);

typedef struct
{
    char *message;
    int message_len;
} ServerResp;

ServerResp *read_from_server(int sock_fd);

ReplyResponse *extract_reply(char *response);
ErrResponse *extract_err(char *response);
AuthResponse *extract_auth(char *response);
JoinResponse *extract_join(char *response);
MsgResponse *extract_msg(char *response);

#endif