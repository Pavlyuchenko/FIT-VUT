#ifndef UDP_HANDLER_H
#define UDP_HANDLER_H

#include <stdint.h>
#include <stddef.h>

#include "fsm_udp.h"

// generation of messages to be sent to the server
int construct_confirm_udp(char *message, int ref_message_id);
int construct_reply_udp(char *message, unsigned char *message_id, char *result, char *ref_message_id, char *message_content);
int construct_auth_udp(char *message, unsigned char *message_id, char *username, char *display_name, char *password);
int construct_join_udp(char *message, unsigned char *message_id, char *channel_id, char *display_name);
int construct_msg_udp(char *message, unsigned char *message_id, char *display_name, char *message_content);
int construct_err_udp(char *message, unsigned char *message_id, char *display_name, char *message_content);
int construct_bye_udp(char *message, unsigned char *message_id);

// parsing of messages received from the server
typedef struct
{
    int ref_message_id;
} ConfirmResponseUDP;
typedef struct
{
    int message_id;
    int result;
    int ref_message_id;
    char *message_content;
} ReplyResponseUDP;
// message
typedef struct
{
    int message_id;
    char *display_name;
    char *message_content;
} MessageResponseUDP;
// err
typedef struct
{
    int message_id;
    char *display_name;
    char *message_content;
} ErrResponseUDP;
// bye
typedef struct
{
    int message_id;
} ByeResponseUDP;
// message_id
typedef struct
{
    unsigned char message_id[2];
} MessageID;

ConfirmResponseUDP *processConfirmCmdUDP(char *buffer);
ReplyResponseUDP *processReplyCmdUDP(char *buffer);
MessageResponseUDP *processMsgCmdUDP(char *buffer);
ErrResponseUDP *processErrCmdUDP(char *buffer);
ByeResponseUDP *processByeCmdUDP(char *buffer);

MessageID *processMessageID(char *buffer);
bool check_received_message_id(unsigned char *message_id);
void add_received_message_id(unsigned char *message_id);

void udp_send(int sock_fd, CLIArguments args, const char *data, unsigned int data_length);
void udp_send2(int sock_fd, CLIArguments args, const char *data, unsigned int data_length);

#endif // UDP_HANDLER_H