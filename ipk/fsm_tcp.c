#include "fsm_tcp.h"

FSMState curr_state = START;

void fsm_tcp(int i, struct epoll_event events[], int sock_fd)
{
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
            curr_state = END;
        }
        else
        {
            // remove newline
            buffer[strcspn(buffer, "\n")] = 0;

            command = parse_command(buffer);
        }
    }

    switch (curr_state)
    {
    case START:
        if (events[i].data.fd == sock_fd)
        {
            send(sock_fd, "BYE\r\n", 5, 0);
            curr_state = END;
        }
        else if (events[i].data.fd == STDIN_FILENO)
        {
            if (command == MESSAGE)
            {
                fprintf(stderr, "ERR: You cannot send messages before authentication.\n");
            }
            else if (command != AUTHENTICATE)
            {
                fprintf(stderr, "ERR: First, you have to authenticate.\n");
            }
            else
            {
                if (authenticate(buffer, sock_fd) == 0)
                {
                    curr_state = AUTH;
                }
            }
        }
        break;
    case AUTH:
        if (events[i].data.fd == sock_fd)
        {
            ServerResp *response = read_from_server(sock_fd);
            if (strncasecmp(response->message, "REPLY", 5) == 0)
            {
                ReplyResponse *reply = extract_reply(response->message);

                if (reply->is_ok)
                {
                    fprintf(stderr, CL_REPLY_SUCCESS, reply->message_content);
                    curr_state = OPEN;
                }
                else
                {
                    fprintf(stderr, CL_REPLY_FAILURE, reply->message_content);
                    curr_state = START;
                }

                free(reply);
            }
            else if (strncasecmp(response->message, "ERR", 3) == 0)
            {
                ErrResponse *err = extract_err(response->message);
                fprintf(stderr, CL_ERROR, err->display_name, err->message_content);
                send(sock_fd, "BYE\r\n", 5, 0);

                free(err);
            }
            else
            {
                // Error
                // TODO: send BYE
            }
        }
        else if (events[i].data.fd == STDIN_FILENO)
        {
            // Ignore
        }
        break;
    case OPEN:
        if (events[i].data.fd == sock_fd)
        {
            ServerResp *response = read_from_server(sock_fd);
            if (strncasecmp(response->message, "MSG", 3) == 0)
            {
                MsgResponse *resp = extract_msg(response->message);
                printf(CL_MESSAGE, resp->display_name, resp->message_content);

                free(resp);
            }
            else if (strncasecmp(response->message, "ERR", 3) == 0)
            {
                ErrResponse *err = extract_err(response->message);
                fprintf(stderr, CL_ERROR, err->display_name, err->message_content);

                free(err);
            }
            else if (strncasecmp(response->message, "REPLY", 5) == 0)
            {
                ReplyResponse *reply = extract_reply(response->message);
                if (reply->is_ok)
                {
                    fprintf(stderr, CL_REPLY_SUCCESS, reply->message_content);
                }
                else
                {
                    fprintf(stderr, CL_REPLY_FAILURE, reply->message_content);
                }

                free(reply);
            }
            else
            {
                fprintf(stderr, "ERR: Unknown command.\n");
                // send ERR FROM {DISPLAYNAME} is {message},
                char err_message[600];
                snprintf(err_message, sizeof(err_message), "ERR FROM %s IS Unknown command\r\n", DISPLAYNAME);
                send(sock_fd, err_message, strlen(err_message), 0);
            }
        }
        else if (events[i].data.fd == STDIN_FILENO)
        {
            if (command == MESSAGE)
            {
                char message[1000];
                snprintf(message, 1000, MSG_PLACEHOLDER, DISPLAYNAME, buffer);
                send(sock_fd, message, strlen(message), 0);
            }
            else if (command == JOIN)
            {
                // /join channel_id
                strtok(buffer, " "); // skip /join
                char *channel_id = strtok(NULL, " ");

                char join_message[600];
                snprintf(join_message, sizeof(join_message), JOIN_MESSAGE_PLACEHOLDER, channel_id, DISPLAYNAME);
                send(sock_fd, join_message, strlen(join_message), 0);
            }
            else if (command == RENAME)
            {
                strtok(buffer, " "); // skip the command
                char *new_display_name = strtok(NULL, " ");
                // change DISPLAYNAME
                strcpy(DISPLAYNAME, new_display_name);
            }
            else if (command == AUTHENTICATE)
            {
                fprintf(stderr, "ERR: You are already authenticated.\n");
            }
            else
            {
                show_help();
            }
        }
        break;
    case ERROR:
        break;
    case END:
        printf("BYE\r\n");
        send(sock_fd, "BYE\r\n", 5, 0);
        close(sock_fd);
        close(STDIN_FILENO);
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
}