#include "fsm_udp.h"
#include <stdarg.h>

// two hex bytes for message_id. global variable
unsigned char message_id[2] = {0x00, 0x00};

void increment_message_id()
{
    if (message_id[1] == 0xFF)
    {
        message_id[0]++;
        message_id[1] = 0x00;
    }
    else
    {
        message_id[1]++;
    }
}

int udp_message_hex(UDPMessageType mess_type)
{
    switch (mess_type)
    {
    case CONFIRM_UDP:
        return 0x00;
    case REPLY_UDP:
        return 0x01;
    case AUTH_UDP:
        return 0x02;
    case JOIN_UDP:
        return 0x03;
    case MSG_UDP:
        return 0x04;
    case ERR_UDP:
        return 0xFE;
    case BYE_UDP:
        return 0xFF;
    }

    return -1;
}

void send_confirm_udp(int sock_fd, CLIArguments args, int message_id)
{
    char confirm[3];
    int len = construct_confirm_udp(confirm, message_id);

    udp_send(sock_fd, args, confirm, len);
}

void send_bye_udp(int sock_fd, CLIArguments args)
{
    char bye[3];
    int len = construct_bye_udp(bye, message_id);

    udp_send(sock_fd, args, bye, len);
}

void send_err_udp(int sock_fd, CLIArguments args)
{
    char err[1000];
    int len = construct_err_udp(err, message_id, DISPLAYNAME, "Error message");

    udp_send(sock_fd, args, err, len);
}

FSMState curr_state_u = START;

void fsm_udp(int i, struct epoll_event events[], int sock_fd, CLIArguments args)
{
    // response buffer if socket
    char response[MAX_BUFFER_SIZE];

    // load buffer if stdin
    char buffer[256];
    UserCommand command;
    memset(buffer, 0, sizeof(buffer));
    if (events[i].data.fd == STDIN_FILENO)
    {
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        // if stdin was closed or EOF
        if (bytes_read == 0 || bytes_read == -1)
        {
            curr_state_u = END;
        }
        else
        {
            // remove newline
            buffer[strcspn(buffer, "\n")] = 0;

            command = parse_command(buffer);
        }
    }
    else if (events[i].data.fd == sock_fd)
    {
        // get response from server and check if port switching is needed
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int bytes_received = recvfrom(sock_fd, response, 100, MSG_WAITALL, (struct sockaddr *)&client_addr, &client_addr_len);
        unsigned short client_port = ntohs(client_addr.sin_port);

        args.server_port = client_port;

        if (bytes_received <= 0)
        {
            curr_state_u = END;
        }

        if (response[0] != 0x00)
        {
            MessageID *message_id_struct = processMessageID(response);
            if (check_received_message_id(message_id_struct->message_id))
            {
                return;
            }
            else
            {
                add_received_message_id(message_id_struct->message_id);
            }
        }
    }

    /*strcpy(DISPLAYNAME, "michal");
    curr_state_u = OPEN;*/
    switch (curr_state_u)
    {
    case START:
        if (events[i].data.fd == sock_fd)
        {
            if (response[0] == REPLY_UDP)
            {
                ReplyResponseUDP *reply_struct = processReplyCmdUDP(response);

                if (reply_struct->result == 1)
                {
                    fprintf(stderr, "Success: %s\n", reply_struct->message_content);

                    send_confirm_udp(sock_fd, args, reply_struct->message_id);

                    curr_state_u = OPEN;
                }
                else
                {
                    fprintf(stderr, "Failure: %s\n", reply_struct->message_content);

                    send_confirm_udp(sock_fd, args, reply_struct->message_id);
                }
            }
            else if ((unsigned char)response[0] == ERR_UDP)
            {
                ErrResponseUDP *err_struct = processErrCmdUDP(response);

                fprintf(stderr, "ERR FROM %s: %s\n", err_struct->display_name, err_struct->message_content);

                send_confirm_udp(sock_fd, args, err_struct->message_id);

                send_bye_udp(sock_fd, args);

                curr_state_u = END;
            }
            else if (response[0] == CONFIRM_UDP)
            {
            }
            else
            {
                send_bye_udp(sock_fd, args);
                curr_state_u = END;
            }
        }
        else if (events[i].data.fd == STDIN_FILENO)
        {
            if (command == AUTHENTICATE)
            {
                AuthResponse *auth_struct = processAuthCmd(buffer);

                if (auth_struct->display_name == NULL || auth_struct->username == NULL || auth_struct->password == NULL)
                {
                    fprintf(stderr, "ERR: Username, password and display name cannot be empty.\n");
                    break;
                }
                strcpy(DISPLAYNAME, auth_struct->display_name);

                char byte_array[256];

                int len = construct_auth_udp(byte_array, message_id, auth_struct->username, auth_struct->display_name, auth_struct->password);
                increment_message_id();

                udp_send(sock_fd, args, byte_array, len);
            }
            else
            {
                fprintf(stderr, "ERR: First, you have to authenticate.\n");
            }
        }
        break;
    case AUTH:
        if (events[i].data.fd == sock_fd)
        {
        }
        else if (events[i].data.fd == STDIN_FILENO)
        {
        }
        break;
    case OPEN:
        if (events[i].data.fd == sock_fd)
        {
            if (response[0] == MSG_UDP)
            {
                MessageResponseUDP *msg_struct = processMsgCmdUDP(response);

                fprintf(stdout, "%s: %s\n", msg_struct->display_name, msg_struct->message_content);

                // send confirm
                send_confirm_udp(sock_fd, args, msg_struct->message_id);
            }
            else if ((unsigned char)response[0] == ERR_UDP)
            {
                ErrResponseUDP *err_struct = processErrCmdUDP(response);

                fprintf(stderr, "ERR FROM %s: %s\n", err_struct->display_name, err_struct->message_content);

                send_confirm_udp(sock_fd, args, err_struct->message_id);

                send_bye_udp(sock_fd, args);
                curr_state_u = END;
            }
            else if (response[0] == REPLY_UDP)
            {
                ReplyResponseUDP *reply_struct = processReplyCmdUDP(response);

                if (reply_struct->result == 1)
                {
                    fprintf(stderr, "Success: %s\n", reply_struct->message_content);

                    send_confirm_udp(sock_fd, args, reply_struct->message_id);

                    curr_state_u = OPEN;
                }
                else
                {
                    fprintf(stderr, "Failure: %s\n", reply_struct->message_content);

                    send_confirm_udp(sock_fd, args, reply_struct->message_id);
                }
            }
            else if (response[0] == CONFIRM_UDP)
            {
            }
            else if ((unsigned char)response[0] == BYE_UDP)
            {
                curr_state_u = END;
            }
            else
            {
                fprintf(stderr, "ERR: Server sent invalid message.\n");

                ConfirmResponseUDP *confirm_struct = processConfirmCmdUDP(response);

                send_confirm_udp(sock_fd, args, confirm_struct->ref_message_id);

                send_err_udp(sock_fd, args);

                send_bye_udp(sock_fd, args);
                curr_state_u = END;
            }
        }
        else if (events[i].data.fd == STDIN_FILENO)
        {
            if (command == MESSAGE)
            {
                char message[1000];

                int len = construct_msg_udp(message, message_id, DISPLAYNAME, buffer);
                increment_message_id();

                udp_send(sock_fd, args, message, len);
            }
            else if (command == RENAME)
            {
                strtok(buffer, " "); // skip /rename
                char *new_display_name = strtok(NULL, " ");
                strcpy(DISPLAYNAME, new_display_name);
            }
            else if (command == JOIN)
            {
                strtok(buffer, " "); // skip /join
                char *channel_id = strtok(NULL, " ");
                char join_message[600];
                int len = construct_join_udp(join_message, message_id, channel_id, DISPLAYNAME);
                increment_message_id();

                udp_send(sock_fd, args, join_message, len);
            }
            else if (command == AUTHENTICATE)
            {
                fprintf(stderr, "ERR: Cannot authenticate twice.\n");
            }
        }
        break;
    case ERROR:
        break;
    case END:
        break;
    default:
        break;
    }
}