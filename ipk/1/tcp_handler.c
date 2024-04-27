#include "tcp_handler.h"

int authenticate(char *buffer, int sock_fd)
{
    AuthResponse *auth_response = processAuthCmd(buffer);

    // Check if any of the fields are NULL
    if (auth_response->username == NULL || auth_response->display_name == NULL || auth_response->password == NULL)
    {
        fprintf(stderr, "ERR: Invalid /auth command. Usage: /auth <username> <password> <display_name>\n");
        return -1;
    }

    // Assuming DISPLAYNAME is a global variable defined in tcp_handler.h
    strcpy(DISPLAYNAME, auth_response->display_name);

    // Send the authentication message to the server
    char auth_message[500];
    snprintf(auth_message, sizeof(auth_message), AUTH_MESSAGE_PLACEHOLDER, auth_response->username, auth_response->display_name, auth_response->password);

    send(sock_fd, auth_message, strlen(auth_message), 0);

    return 0;
}

char *get_ip(char *server_ip)
{
    struct sockaddr_in res_ip;

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *result;

    if (getaddrinfo(server_ip, NULL, &hints, &result) != 0)
    {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    memcpy(&res_ip, result->ai_addr, result->ai_addrlen);

    return inet_ntoa(res_ip.sin_addr);
}

ReplyResponse *extract_reply(char *response)
{
    strtok(response, " ");                // REPLY
    char *status = strtok(NULL, " ");     // OK | NOK
    strtok(NULL, " ");                    // IS
    char *message = strtok(NULL, "\r\n"); // MessageContent

    ReplyResponse *reply_struct = malloc(sizeof(ReplyResponse));
    reply_struct->is_ok = strcmp(status, "OK") == 0;
    strcpy(reply_struct->message_content, message);

    return reply_struct;
}

ErrResponse *extract_err(char *response)
{
    strtok(response, " "); // ERR
    strtok(NULL, " ");     // FROM
    char *displayName = strtok(NULL, " ");
    strtok(NULL, " "); // IS
    char *messageContent = strtok(NULL, "\r\n");

    ErrResponse *err_struct = malloc(sizeof(ErrResponse));
    err_struct->display_name = displayName;
    strcpy(err_struct->message_content, messageContent);

    return err_struct;
}

AuthResponse *extract_auth(char *response)
{
    strtok(response, " "); // AUTH
    char *username = strtok(NULL, " ");
    strtok(NULL, " "); // AS
    char *displayName = strtok(NULL, " ");
    strtok(NULL, " "); // USING
    char *password = strtok(NULL, "\r\n");

    AuthResponse *auth_struct = malloc(sizeof(AuthResponse));
    auth_struct->display_name = displayName;
    auth_struct->username = username;
    auth_struct->password = password;

    return auth_struct;
}

JoinResponse *extract_join(char *response)
{
    strtok(response, " "); // JOIN
    char *channelID = strtok(NULL, " ");
    strtok(NULL, " "); // AS
    char *displayName = strtok(NULL, "\r\n");

    JoinResponse *join_struct = malloc(sizeof(JoinResponse));
    join_struct->channel_id = channelID;
    join_struct->display_name = displayName;

    return join_struct;
}

MsgResponse *extract_msg(char *response)
{
    strtok(response, " "); // MSG
    strtok(NULL, " ");     // FROM
    char *displayName = strtok(NULL, " ");
    strtok(NULL, " "); // IS
    char *messageContent = strtok(NULL, "\r\n");

    MsgResponse *msg_struct = malloc(sizeof(MsgResponse));
    msg_struct->display_name = displayName;
    strcpy(msg_struct->message_content, messageContent);

    return msg_struct;
}

ServerResp *read_from_server(int sock_fd)
{
    char response[MAX_BUFFER_SIZE];
    int bytes_received = recv(sock_fd, response, MAX_BUFFER_SIZE, 0);
    if (bytes_received <= 0)
    {
        printf("Server disconnected\n");
        // TODO: curr_state = END;
    }

    ServerResp *resp = malloc(sizeof(ServerResp));
    resp->message = response;
    resp->message_len = bytes_received;

    return resp;
}