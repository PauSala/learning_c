#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/event.h>

#include "../include/logger.h"
#include "../include/errors.h"
#include "../include/handler.h"

#define SERVER_BACKLOG 2048
#define MAX_EVENTS 2048
#define BUFFER_SIZE 1024

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
        // Lose the "address already in use" error message
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
    if (listen(listener, SERVER_BACKLOG) == -1)
    {
        return -1;
    }
    return listener;
}

int main(void)
{
    int listener; // Listening socket descriptor
    int newfd;    // Newly accepted socket
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];

    // Create kqueue
    int kq = kqueue();
    if (kq == -1)
    {
        perror("kqueue");
        exit(1);
    }

    // Get a listening socket
    listener = get_listener_socket();
    if (listener == -1)
    {
        logger("Error getting listening socket", ERROR);
        exit(1);
    }

    // Make the listener socket non-blocking
    fcntl(listener, F_SETFL, O_NONBLOCK);

    // Add listener socket to kqueue
    logger("Adding listener", DEBUG);
    add_event(kq, listener, EVFILT_READ, EV_ADD);

    struct kevent events[MAX_EVENTS];

    // Main event loop
    for (;;)
    {
        int nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        logger("\n\n-------------- Got %d events--------------------\n", DEBUG, nev);
        if (nev == -1)
        {
            perror("kevents");
            exit(1);
        }

        for (int i = 0; i < nev; i++)
        {
            int fd = (int)events[i].ident;
            if (fd == listener)
            {
                // Accept new connection
                addrlen = sizeof remoteaddr;
                newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
                if (newfd == -1)
                {
                    perror("accept");
                }
                else
                {
                    // Make new socket non-blocking
                    fcntl(newfd, F_SETFL, O_NONBLOCK);
                    logger("New connection  on socket %d", DEBUG, newfd);
                    add_event(kq, newfd, EVFILT_READ, EV_ADD | EV_ENABLE);
                }
            }
            else if (events[i].flags & EV_EOF)
            {
                clean_up(&events[i], fd);
                continue;
            }

            else if (events[i].filter == EVFILT_READ)
            {
                logger("EVFILT_READ i: %d socket: %d | data: %ld", DEBUG, i, fd, events[i].data);
                handle_request(&events[i], fd, kq,
                               inet_ntop(remoteaddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN));
            }
            else if (events[i].filter == EVFILT_WRITE)
            {
                logger("EVFILT_WRITE socket: %d", DEBUG, fd);
                handle_response(&events[i], fd, kq,
                                inet_ntop(remoteaddr.ss_family,
                                          get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN));
            }
        }
    }

    logger("Exit", INFO);
    close(listener);
    return 0;
}
