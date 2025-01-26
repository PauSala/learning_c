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
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <time.h>

#include "../include/html_res.h"
#include "../include/response_t.h"
#include "../include/logger.h"
#include "../include/errors.h"
#include "../include/handler.h"

#define OK_200 "HTTP/1.1 200 OK\r\nContent-Length:2\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nOK"
#define BAD_REQUEST_400 "HTTP/1.1 400 Bad Request\r\nContent-Length:11\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nBad Request"

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
    if (listen(listener, 4096) == -1)
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
    // char remoteIP[INET6_ADDRSTRLEN];

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
    logger("Adding listener", INFO);
    add_event(kq, listener, EVFILT_READ, EV_ADD);

    struct kevent events[MAX_EVENTS];

    // Main event loop
    for (;;)
    {
        int nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        printf("\n\n-------------- Got %d events--------------------\n", nev);
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
                    printf("New connection  on socket %d\n", newfd);
                    add_event(kq, newfd, EVFILT_READ, EV_ADD | EV_ENABLE);
                }
            }
            else if (events[i].flags & EV_EOF)
            {
                printf("EOF: %d fflags: %d\n", fd, events[i].fflags);
                close(fd);
                continue;
            }

            else if (events[i].filter == EVFILT_READ)
            {
                // add_event(kq, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
                printf("EVFILT_READ socket: %d | data: %ld\n", fd, events[i].data);

                char data[events[i].data > 0 ? events[i].data : 1];
                int nbytes = recv(fd, data, events[i].data, 0);
                if (nbytes < 0)
                {
                    if (errno == EWOULDBLOCK || errno == EAGAIN)
                    {
                        printf("Read: Operation would block, try again later.\n");
                        // Just to see if this ever happens
                        exit(1);
                    }
                    else
                    {
                        perror("read");
                        close(fd);
                    }
                }
                else if (nbytes == 0)
                {
                    printf("Read: Connection closed by peer.\n");
                    close(fd);
                }
                else
                {
                    printf("Read %d bytes.\n", nbytes);
                    printf("Received data: %.*s\n", nbytes, data);
                    add_event(kq, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
                    add_event(kq, fd, EVFILT_READ, EV_DELETE);
                }
            }
            else if (events[i].filter == EVFILT_WRITE)
            {
                printf("EVFILT_WRITE socket: %d \n", fd);
                int response_len = strlen(OK_200);
                int nbytes = send(fd, OK_200, response_len, 0);
                if (nbytes < 0)
                {
                    if (errno == EWOULDBLOCK || errno == EAGAIN)
                    {
                        printf("Send: Operation would block, try again later.\n");
                        // Just to see if this ever happens
                        exit(1);
                    }
                    else
                    {
                        perror("write");
                        close(fd);
                    }
                }
                else if (nbytes == 0)
                {
                    printf("Write: Connection closed by peer.\n");
                    close(fd);
                }
                else if (nbytes < response_len)
                {
                    printf("Write: not all data is writen.\n");
                    // Just to see if this ever happens
                    // TODO: handle
                    exit(1);
                }
                else
                {
                    close(fd);
                }
            }
        }
    }

    logger("Exit", INFO);
    close(listener);
    return 0;
}
