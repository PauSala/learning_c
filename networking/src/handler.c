#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/event.h>

#include "../include/logger.h"
#include "../include/response_t.h"
#include "../include/html_res.h"
#include "../include/handler.h"
#include "../include/parse_request.h"

#define ERR_413 "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/plain\r\nContent-Length: 42\r\nConnection: close\r\n\r\nPayload Too Large. Request entity too big."
#define ERR_500 "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 44\r\nConnection: close\r\n\r\nInternal Server Error. Something went wrong."

#define INITIAL_BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 65536

// Hash size
#define HASH_SIZE 1024

// Hash table for connections
PendingResponse *connection_table[HASH_SIZE] = {NULL};

int hash_fd(int fd)
{
    return fd % HASH_SIZE;
}

void insert_new_connection(int fd, char *data, char *start)
{
    int index = hash_fd(fd);
    PendingResponse *new_response = malloc(sizeof(PendingResponse));
    new_response->fd = fd;
    new_response->data = data;
    new_response->start = start;
    new_response->active = 1;
    new_response->next = connection_table[index];
    connection_table[index] = new_response;
}

PendingResponse *find_connection(int fd)
{
    int index = hash_fd(fd);
    PendingResponse *curr = connection_table[index];
    while (curr)
    {
        if (curr->fd == fd)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_event(int kq, int fd, int filter, int flags)
{
    struct kevent ev;
    EV_SET(&ev, fd, filter, flags, 0, 0, NULL);
    if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
    {
        perror("kevent add");
        exit(1);
    }
}

void handle_request(int fd, int kq, const char *client_ip)
{
    size_t buf_size = INITIAL_BUFFER_SIZE;
    char *buf = malloc(buf_size);
    if (!buf)
    {
        critical_logger("failed to allocate");
        add_event(kq, fd, EVFILT_READ, EV_DELETE);
        return;
    }

    size_t total_bytes = 0;
    ssize_t nbytes;

    while ((nbytes = recv(fd, buf + total_bytes, buf_size - total_bytes - 1, 0)) > 0)
    {
        total_bytes += nbytes;
        buf[total_bytes] = '\0';

        if (total_bytes >= buf_size - 1)
        {
            if (buf_size >= MAX_REQUEST_SIZE)
            {
                logger("Request too large from: %s, closing connection.", ERROR, client_ip);
                free(buf);
                send(fd, ERR_413, strlen(ERR_413), 0);
                close(fd);
                return;
            }

            size_t new_size = buf_size * 2; // Double the buffer size
            char *new_buf = realloc(buf, new_size);
            if (!new_buf)
            {
                critical_logger("realloc failed");
                free(buf);
                close(fd);
                return;
            }

            buf = new_buf;
            buf_size = new_size;
        }
    }

    if (nbytes == 0)
    {
        logger("Client disconnected: %s", INFO, client_ip);
        add_event(kq, fd, EVFILT_READ, EV_DELETE);
        return;
    }
    else if (nbytes == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        char *err = strdup(strerror(errno));
        logger("recv: %s", ERROR, err);
        free(err);
        add_event(kq, fd, EVFILT_READ, EV_DELETE);
        return;
    }

    logger("Start parsing: %d\n", INFO, strlen(buf));

    Span body = {.start = 0, .len = 0};
    Span url = {.start = 0, .len = 0};
    Span version = {.start = 0, .len = 0};
    HttpHeadersArray headers = {NULL, 0, 0};
    HttpRequest req = {.headers = &headers, .body = body, .url = url, .version = version};
    HttpParser parser = {
        .input_len = strlen(buf),
        .state = START,
        .start = 0,
        .curr = 0,
        .request = &req};
    parse_request(&parser, buf);
    http_request_to_string(&parser, buf);

    // Add event to write response
    add_event(kq, fd, EVFILT_WRITE, EV_ADD);
    free(buf);
}

void handle_response(int fd, int kq, const char *client_ip)
{
    ssize_t bytes_sent;

    // Is there already a connection waiting for more data to be read?
    PendingResponse *old = find_connection(fd);
    if (old != NULL && old->active)
    {
        logger("Reading %d bytes of pending data.", DEBUG, strlen(old->data));
        bytes_sent = send(fd, old->data, strlen(old->data), 0);
        if (bytes_sent < (int)strlen(old->data))
        {
            old->data = old->data + bytes_sent;
            return;
        }
        else
        {
            logger("Sending pending data has worked well.", DEBUG);
            old->active = 0;
            old->data = NULL;
            old->start = NULL;
            add_event(kq, fd, EVFILT_WRITE, EV_DELETE);
            return;
        }
    }

    ResultChar response = html_response("hello.html");
    if (response.ty == Ok)
    {
        bytes_sent = send(fd, response.val.res, strlen(response.val.res), 0);
        if (bytes_sent < (int)strlen(response.val.res))
        {
            logger("Data does not fit in response: %d, bytes_sent: %d", DEBUG, strlen(response.val.res), bytes_sent);
            if (old)
            {
                logger("And old item exists and will be used", DEBUG);
                old->start = response.val.res;
                old->data = response.val.res + bytes_sent;
                old->active = 1;
                old->fd = fd;
            }
            else
            {
                logger("insert_new_connection", DEBUG);
                insert_new_connection(fd, response.val.res + bytes_sent, response.val.res);
            }
            return;
        }
        else
        {
            logger("All data fits in response", DEBUG);
        }
    }
    else
    {
        logger("%s %s", ERROR, e_to_string(&response.val.err), "Error getting html_response.");
        send(fd, ERR_500, strlen(ERR_500), 0);
    }

    logger("Closing connection after write from %s", INFO, client_ip);

    // // Remove filters
    // add_event(kq, fd, EVFILT_READ, EV_DELETE);
    add_event(kq, fd, EVFILT_WRITE, EV_DELETE);
    // // Close the file descriptor
    close(fd);
}
