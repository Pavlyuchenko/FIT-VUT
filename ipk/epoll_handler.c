#include "epoll_handler.h"

int get_epoll_fd()
{
    struct epoll_event event;
    int epoll_fd, fd_stdin;

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Also add stdin to epoll
    fd_stdin = STDIN_FILENO;
    // Set stdin to non-blocking
    fcntl(fd_stdin, F_SETFL, O_NONBLOCK);
    // Add stdin to epoll
    event.events = EPOLLIN | EPOLLET; // Edge-triggered mode
    event.data.fd = fd_stdin;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_stdin, &event);

    return epoll_fd;
}

int get_sock_fd(CLIArguments args, int epoll_fd)
{
    // SOCK_STREAM for TCP, SOCK_DGRAM for UDP TODO:
    if (args.transport_protocol == TCP)
    {
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == -1)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        struct epoll_event sock_event;

        sock_event.events = EPOLLIN;
        sock_event.data.fd = sock_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &sock_event) == -1)
        {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(args.server_port);
        char *resolved_ip = get_ip(args.server_ip);
        server_addr.sin_addr.s_addr = inet_addr(resolved_ip);

        int connect_result = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (connect_result == -1)
        {
            perror("connect");
            exit(EXIT_FAILURE);
        }

        return sock_fd;
    }
    else if (args.transport_protocol == UDP)
    {
        int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd == -1)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        struct epoll_event sock_event;

        sock_event.events = EPOLLIN;
        sock_event.data.fd = sock_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &sock_event) == -1)
        {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }

        return sock_fd;
    }
    else
    {
        // should not happen
        fprintf(stderr, "Invalid transport protocol\n");
        exit(EXIT_FAILURE);
    }
}
