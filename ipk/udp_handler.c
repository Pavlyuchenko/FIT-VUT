#include "udp_handler.h"

// CONFIRM |  0x00  |  Ref_MessageID  |
int construct_confirm_udp(char *message, int ref_message_id)
{
    message[0] = 0x00; // message_id high byte

    message[1] = ref_message_id >> 8;
    message[2] = ref_message_id & 0xFF;

    return 3;
}

// REPLY |  0x01  |    MessageID    | Result |  Ref_MessageID  |  MessageContents  | 0 |
int construct_reply_udp(char *message, unsigned char *message_id, char *result, char *ref_message_id, char *message_content)
{
    message[0] = 0x01; // message_id high byte

    message[1] = message_id[0]; // message_id low byte
    message[2] = message_id[1]; // message_id low byte

    message[3] = result[0]; // result

    message[4] = ref_message_id[0]; // ref_message_id high byte
    message[5] = ref_message_id[1]; // ref_message_id low byte

    // construct byte_array dynamically
    int byte_array_index = 6;
    for (size_t i = 0; i < strlen(message_content); i++)
    {
        message[byte_array_index] = message_content[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;

    return byte_array_index + 1;
}

// AUTH: |  0x02  |    MessageID    |  Username  | 0 |  DisplayName  | 0 |  Secret  | 0 |
int construct_auth_udp(char *message, unsigned char *message_id, char *username, char *display_name, char *password)
{
    message[0] = 0x02; // message_id high byte

    message[1] = message_id[0]; // message_id low byte
    message[2] = message_id[1]; // message_id low byte

    // construct byte_array dynamically
    int byte_array_index = 3;
    for (size_t i = 0; i < strlen(username); i++)
    {
        message[byte_array_index] = username[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;
    byte_array_index++;
    for (size_t i = 0; i < strlen(display_name); i++)
    {
        message[byte_array_index] = display_name[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;
    byte_array_index++;
    for (size_t i = 0; i < strlen(password); i++)
    {
        message[byte_array_index] = password[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;

    return byte_array_index + 1;
}

// JOIN: |  0x03  |    MessageID    |  ChannelID | 0 |  DisplayName  | 0 |
int construct_join_udp(char *message, unsigned char *message_id, char *channel_id, char *display_name)
{
    message[0] = 0x03; // message_id high byte

    message[1] = message_id[0]; // message_id low byte
    message[2] = message_id[1]; // message_id low byte

    // construct byte_array dynamically
    int byte_array_index = 3;
    for (size_t i = 0; i < strlen(channel_id); i++)
    {
        message[byte_array_index] = channel_id[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;
    byte_array_index++;
    for (size_t i = 0; i < strlen(display_name); i++)
    {
        message[byte_array_index] = display_name[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;

    return byte_array_index + 1;
}

// MSG: |  0x04  |    MessageID    |  DisplayName  | 0 |  MessageContents  | 0 |
int construct_msg_udp(char *message, unsigned char *message_id, char *display_name, char *message_content)
{
    message[0] = 0x04; // message_id high byte

    message[1] = message_id[0]; // message_id low byte
    message[2] = message_id[1]; // message_id low byte

    // construct byte_array dynamically
    int byte_array_index = 3;
    for (size_t i = 0; i < strlen(display_name); i++)
    {
        message[byte_array_index] = display_name[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;
    byte_array_index++;
    for (size_t i = 0; i < strlen(message_content); i++)
    {
        message[byte_array_index] = message_content[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;

    return byte_array_index + 1;
}

// ERR: |  0xFE  |    MessageID    |  DisplayName  | 0 |  MessageContents  | 0 |
int construct_err_udp(char *message, unsigned char *message_id, char *display_name, char *message_content)
{
    message[0] = (unsigned char)0xFE; // message_id high byte

    message[1] = message_id[0]; // message_id low byte
    message[2] = message_id[1]; // message_id low byte

    // construct byte_array dynamically
    int byte_array_index = 3;
    for (size_t i = 0; i < strlen(display_name); i++)
    {
        message[byte_array_index] = display_name[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;
    byte_array_index++;
    for (size_t i = 0; i < strlen(message_content); i++)
    {
        message[byte_array_index] = message_content[i];
        byte_array_index++;
    }
    message[byte_array_index] = 0x00;

    return byte_array_index + 1;
}

// BYE |  0xFF  |    MessageID    |
int construct_bye_udp(char *message, unsigned char *message_id)
{
    message[0] = (unsigned char)0xFF; // message_id high byte

    message[1] = message_id[0]; // message_id low byte
    message[2] = message_id[1]; // message_id low byte

    return 3;
}

// array of received message_ids
unsigned char received_message_ids[100][2];
int received_message_ids_index = 0;

bool check_received_message_id(unsigned char *message_id)
{
    // print message_id
    for (int i = 0; i < received_message_ids_index; i++)
    {
        if (received_message_ids[i][0] == message_id[0] && received_message_ids[i][1] == message_id[1])
        {
            return true;
        }
    }

    return false;
}

void add_received_message_id(unsigned char *message_id)
{
    received_message_ids[received_message_ids_index][0] = message_id[0];
    received_message_ids[received_message_ids_index][1] = message_id[1];
    received_message_ids_index++;
}

MessageID *processMessageID(char *buffer)
{
    MessageID *message_id = malloc(sizeof(MessageID));

    // buffer : | TYPE |    MessageID    |
    // MessageID: uint16
    message_id->message_id[0] = buffer[1];
    message_id->message_id[1] = buffer[2];

    return message_id;
}

// response parsing
ConfirmResponseUDP *processConfirmCmdUDP(char *buffer)
{
    ConfirmResponseUDP *confirm_response = malloc(sizeof(ConfirmResponseUDP));
    // buffer : |  0x00  |  Ref_MessageID  |
    // Ref_MessageID: uint16
    confirm_response->ref_message_id = (buffer[1] << 8) | buffer[2];

    received_message_ids[received_message_ids_index][0] = buffer[1];
    received_message_ids[received_message_ids_index][1] = buffer[2];
    received_message_ids_index++;

    return confirm_response;
}

ReplyResponseUDP *processReplyCmdUDP(char *buffer)
{
    ReplyResponseUDP *reply_response = malloc(sizeof(ReplyResponseUDP));

    // buffer : |  0x01  |    MessageID    | Result |  Ref_MessageID  |  MessageContents  | 0 |
    // result: 0 | 1
    // Ref_MessageID: uint16
    // MessageContents: string
    reply_response->message_id = (buffer[1] << 8) | buffer[2];
    reply_response->result = (int)(buffer[3]);
    reply_response->ref_message_id = (buffer[4] << 8) | buffer[5];
    reply_response->message_content = &buffer[6];

    return reply_response;
}

MessageResponseUDP *processMsgCmdUDP(char *buffer)
{
    MessageResponseUDP *msg_response = malloc(sizeof(MessageResponseUDP));

    // buffer : |  0x04  |    MessageID    |  DisplayName  | 0 |  MessageContents  | 0 |
    // MessageID: uint16
    // DisplayName: string
    // MessageContents: string
    msg_response->message_id = (buffer[1] << 8) | buffer[2];
    msg_response->display_name = &buffer[3];
    msg_response->message_content = &buffer[4 + strlen(msg_response->display_name)];

    return msg_response;
}

ErrResponseUDP *processErrCmdUDP(char *buffer)
{
    ErrResponseUDP *err_response = malloc(sizeof(ErrResponseUDP));

    // buffer : |  0xFE  |    MessageID    |  DisplayName  | 0 |  MessageContents  | 0 |
    // MessageID: uint16
    // DisplayName: string
    // MessageContents: string
    err_response->message_id = (buffer[1] << 8) | buffer[2];
    err_response->display_name = &buffer[3];
    err_response->message_content = &buffer[4 + strlen(err_response->display_name)];

    return err_response;
}

void udp_send(int sock_fd, CLIArguments args, const char *data, unsigned int data_length)
{
    struct sockaddr_in s;
    s.sin_family = AF_INET;
    s.sin_port = htons(args.server_port);

    char *resolved_ip = get_ip(args.server_ip);
    if (inet_pton(AF_INET, resolved_ip, &s.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    tv.tv_sec = args.udp_timeout;
    tv.tv_usec = 0;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

    // wait args.udp_timeout seconds for response, else retry. do this for args.udp_retransmission times
    for (int i = 0; i < args.udp_retransmissions; i++)
    {
        sendto(sock_fd, data, data_length, 0, (struct sockaddr *)(&s), sizeof(s));
        if (data[0] == CONFIRM_UDP)
        {
            return;
        }

        // Temporarily modify the receive timeout to 250 ms
        struct timeval recv_timeout;
        recv_timeout.tv_sec = 0;
        // recv_timeout.tv_usec = 250000;
        recv_timeout.tv_usec = args.udp_timeout * 1000;
        setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&recv_timeout, sizeof recv_timeout);

        // recv
        char response[MAX_BUFFER_SIZE];
        recv(sock_fd, response, MAX_BUFFER_SIZE, 0);

        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            continue;
        }

        // check if we received CONFIRM, if so, break, else retry
        if (response[0] == CONFIRM_UDP)
        {
            return;
        }
    }

    exit(EXIT_FAILURE);
}

// Without wait for confirm, which should be handled by different sock_fd
void udp_send2(int sock_fd, CLIArguments args, const char *data, unsigned int data_length)
{
    struct sockaddr_in s;
    s.sin_family = AF_INET;
    s.sin_port = htons(args.server_port);

    char *resolved_ip = get_ip(args.server_ip);
    if (inet_pton(AF_INET, resolved_ip, &s.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    sendto(sock_fd, data, data_length, 0, (struct sockaddr *)(&s), sizeof(s));
}