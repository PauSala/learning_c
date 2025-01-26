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

#define OK_200 "HTTP/1.1 200 OK\r\nContent-Length:2\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nOK"
#define ERR_413 "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/plain\r\nContent-Length: 42\r\nConnection: close\r\n\r\nPayload Too Large. Request entity too big."
#define ERR_500 "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 44\r\nConnection: close\r\n\r\nInternal Server Error. Something went wrong."
#define BAD_REQUEST_400 "HTTP/1.1 400 Bad Request\r\nContent-Length:11\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nBad Request"

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

void handle_request(struct kevent event, int fd, int kq, const char *client_ip)
{

    if (event.data > MAX_REQUEST_SIZE)
    {
        logger("Request too large from %s, closing connection.", ERROR, client_ip);
        send(fd, ERR_413, strlen(ERR_413), 0);
        close(fd);
        return;
    }

    size_t req_len = event.data > 0 ? event.data : 1;
    char *buf = malloc(req_len + 1);
    if (!buf)
    {
        perror("malloc");
        close(fd);
        return;
    }

    ssize_t nbytes;
    nbytes = recv(fd, buf, req_len, 0);

    if (nbytes == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        perror("read");
        close(fd);
        return;
    }

    else if (nbytes == 0)
    {
        logger("Connection closed by peer: %s", DEBUG, client_ip);
        close(fd);
        return;
    }

    buf[nbytes] = '\0';

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
    add_event(kq, fd, EVFILT_READ, EV_DELETE);
    free(buf);
}

void handle_response(int fd, const char *client_ip)
{
    ssize_t bytes_sent;

    // Is there already a connection waiting for more data to be read?
    PendingResponse *old = find_connection(fd);
    if (old != NULL && old->active)
    {
        logger("Reading %d bytes of pending data.", DEBUG, strlen(old->data));

        bytes_sent = send(fd, old->data, strlen(old->data), 0);
        if (bytes_sent == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("write");
            close(fd);
            return;
        }
        if (bytes_sent == 0)
        {
            logger("Write: Connection closed by peer.", DEBUG);
            close(fd);
            return;
        }
        if (bytes_sent < (int)strlen(old->data))
        {
            old->data = old->data + bytes_sent;
            return;
        }
        logger("All pending data has been sent!", DEBUG);
        old->active = 0;
        old->data = NULL;
        old->start = NULL;
        shutdown(fd, SHUT_WR);
        return;
    }

    ResultChar response = html_response("hello.html");
    if (response.ty == Err)
    {
        logger("%s %s", ERROR, e_to_string(&response.val.err), "Error getting html_response.");
        send(fd, ERR_500, strlen(ERR_500), 0);
        close(fd);
    }

    bytes_sent = send(fd, response.val.res, strlen(response.val.res), 0);
    if (bytes_sent == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        perror("write");
        close(fd);
        return;
    }
    if (bytes_sent == 0)
    {
        logger("Write: Connection closed by peer.", DEBUG);
        close(fd);
        return;
    }
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
    logger("Closing connection after write from %s", DEBUG, client_ip);
    // // Close the file descriptor
    shutdown(fd, SHUT_WR);
    return;
}
