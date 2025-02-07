#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/event.h>

#include "../include/logger.h"
#include "../include/result.h"
#include "../include/html_res.h"
#include "../include/handler.h"
#include "../include/http_parser.h"

#define OK_200 "HTTP/1.1 200 OK\r\nContent-Length:2\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nOK"
#define ERR_413 "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/plain\r\nContent-Length: 42\r\nConnection: close\r\n\r\nPayload Too Large. Request entity too big."
#define ERR_500 "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 44\r\nConnection: close\r\n\r\nInternal Server Error. Something went wrong."
#define BAD_REQUEST_400 "HTTP/1.1 400 Bad Request\r\nContent-Length:11\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nBad Request"

#define INITIAL_BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 65536

// Hash size
#define HASH_SIZE 1024

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

void handle_request(struct kevent *event, int fd, int kq, const char *client_ip)
{

    HttpParser *parser = (HttpParser *)event->udata;

    size_t req_len = event->data > 0 ? event->data : 1;
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
        if (parser)
        {
            free_parser(parser);
        }
        return;
    }

    else if (nbytes == 0)
    {
        logger("Connection closed by peer: %s", DEBUG, client_ip);
        close(fd);
        if (parser)
        {
            free_parser(parser);
        }
        return;
    }

    buf[nbytes] = '\0';

    if (!parser)
    {
        parser = init_parser(buf);
    }
    else
    {
        append_request(parser, buf);
        parser->input_len = parser->input_len + nbytes;
        parser->eof = false;
    }

    parse_request(parser);
    http_request_to_string(parser);

    if (parser->state == END_PARSE || parser->state == PARSER_ERROR)
    {
        add_event(kq, fd, EVFILT_WRITE, EV_ADD);
        add_event(kq, fd, EVFILT_READ, EV_DELETE);
        free_parser(parser);
        return;
    }

    event->udata = parser;
    kevent(kq, event, 1, NULL, 0, NULL);
}

void handle_response(struct kevent *event, int fd, int kq, const char *client_ip)
{
    // ResultChar response = html_response("hello.html");
    ResultChar response = html_response("hello.html");
    if (response.ty == Err)
    {
        logger("%s %s", ERROR, e_to_string(&response.val.err), "Error getting html_response.");
        send(fd, ERR_500, strlen(ERR_500), 0);
        close(fd);
        return;
    }

    PartialWrite *pw = (PartialWrite *)event->udata;
    ssize_t bytes_sent;
    char *data;

    if (pw)
    {
        logger("Some data is pending to write", DEBUG);
        data = response.val.res + pw->bytes_sent;
    }
    else
    {
        data = response.val.res;
    }

    bytes_sent = send(fd, data, strlen(data), 0);

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
    if (bytes_sent < (int)strlen(data))
    {
        if (!pw)
        {
            pw = malloc(sizeof(PartialWrite));
            if (!pw)
            {
                perror("malloc partial write");
                exit(1);
            }
            pw->bytes_sent = 0;
        }
        pw->bytes_sent += bytes_sent;
        event->udata = pw;
        kevent(kq, event, 1, NULL, 0, NULL);
        return;
    }

    if (pw)
    {
        free(pw);
    }
    logger("Closing connection after write from %s", DEBUG, client_ip);

    // TODO: Close or shutdown?
    shutdown(fd, SHUT_WR);
    return;
}

void clean_up(struct kevent *event, int fd)
{
    if (event->filter == EVFILT_READ)
    {
        logger("EOF EVFILT_READ: %d fflags: %d", DEBUG, fd, event->fflags);
        HttpParser *parser = (HttpParser *)event->udata;
        if (parser)
        {
            free_parser(parser);
        }
    }
    if (event->filter == EVFILT_WRITE)
    {
        logger("EOF EVFILT_WRITE: %d fflags: %d", DEBUG, fd, event->fflags);
        PartialWrite *pw = (PartialWrite *)event->udata;
        if (pw)
        {
            free(pw);
        }
    }
    close(fd);
}
