// poll server example from https://beej.vals/guide/bgnet/html/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>

#include "../include/html_res.h"
#include "../include/response_t.h"
#include "../include/logger.h"
#include "../include/errors.h"

#define DEFAULT_ERROR_MESSAGE "THIS IS THE DEFAULT ERROR MESSAGE"
#define PORT "3000"
#define HOST "0.0.0.0"

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    int listener; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(HOST, PORT, &hints, &ai)) != 0)
    {
        logger("Pollserver: %s", ERROR, gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            continue;
        }
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }
        break;
    }

    freeaddrinfo(ai);

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        return -1;
    }

    logger("Listening on %s:%s", INFO, HOST, PORT);

    // Listen
    if (listen(listener, 10) == -1)
    {
        return -1;
    }
    return listener;
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size)
    {
        *fd_size *= 2; // Double it

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}

// Main
int main(void)
{

    int listener; // Listening socket descriptor

    int newfd;                          // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    // Set up and get a listening socket
    listener = get_listener_socket();
    if (listener == -1)
    {
        logger("Error getting listening socket", ERROR);
        exit(1);
    }

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count = 1; // For the listener

    // Main loop
    for (;;)
    {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1)
        {
            char *err = strdup(strerror(errno));
            logger("Poll: %s", ERROR, err);
            free(err);
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (int i = 0; i < fd_count; i++)
        {

            // Check if someone's ready to read
            if (pfds[i].revents & (POLLIN | POLLHUP))
            { // We got one!!
                if (pfds[i].fd == listener)
                {
                    // If listener is ready to read, handle new connection
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                                   (struct sockaddr *)&remoteaddr,
                                   &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {

                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
                        logger("New connection from %s on socket %d", INFO,
                               inet_ntop(remoteaddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                    }
                }
                else
                {
                    // If not the listener, we're just a regular client
                    char buf[1024]; // Buffer to hold client data
                    ssize_t nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0)
                    {
                        // Got error or connection closed by client
                        if (nbytes == 0)
                        {
                            // Connection closed
                            logger("Socket hung up", WARN);
                        }
                        else
                        {
                            perror("recv");
                        }

                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                    }
                    else
                    {
                        // Null-terminate the received data to safely print it
                        buf[nbytes] = '\0';
                        logger("Received data from %s\n%s", INFO,
                               inet_ntop(
                                   remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN),
                               buf);

                        // Default hello world response
                        ResultChar response = html_response("hello.html");
                        if (response.ty == Ok)
                        {
                            send(sender_fd, response.val.res, strlen(response.val.res), 0);
                        }
                        else
                        {
                            logger("%s %s", ERROR, e_to_string(&response.val.err), "When trying to get html_response.");
                        }

                        free_result_char(&response);
                        close(sender_fd);
                    }
                }
            }
        }
    }

    return 0;
}
