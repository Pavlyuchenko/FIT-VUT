#ifndef CLI_H
#define CLI_H

#include <stdint.h>
#include <stddef.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define DEFAULT_SERVER_PORT 4567
#define DEFAULT_UDP_TIMEOUT 250

extern char DISPLAYNAME[256];

// Messages that are displayed to the user based on the server response
#define CL_MESSAGE "%s: %s\n"            // DisplayName, MessageContent
#define CL_ERROR "ERR FROM %s: %s\n"     // DisplayName, MessageContent
#define CL_INTERNAL_ERROR "ERR: %s\n"    // MessageContent
#define CL_REPLY_SUCCESS "Success: %s\n" // MessageContent
#define CL_REPLY_FAILURE "Failure: %s\n" // MessageContent

typedef enum
{
    TCP,
    UDP
} TransportProtocol;

typedef struct
{
    TransportProtocol transport_protocol;
    char *server_ip;
    uint16_t server_port;
    uint16_t udp_timeout;
    uint8_t udp_retransmissions;
} CLIArguments;

/**
 * @brief Parse command line arguments
 */
CLIArguments get_cli_arguments(int argc, char *argv[]);

typedef enum
{
    AUTHENTICATE,
    JOIN,
    RENAME,
    HELP,
    MESSAGE,
    UNKNOWN
} UserCommand;

/**
 * @brief Get user command
 */
UserCommand get_user_command(char *command);

/**
 * @brief Parse user command
 */
UserCommand parse_command(char *command);

void show_help();

// AUTH {Username} AS {DisplayName} USING {Secret}\r\n
typedef struct
{
    char *display_name;
    char *username;
    char *password;
} AuthResponse;

AuthResponse *processAuthCmd(char *buffer);

#endif