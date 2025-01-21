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

#define ERR_413 "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/plain\r\nContent-Length: 42\r\nConnection: close\r\n\r\nPayload Too Large. Request entity too big."

#define INITIAL_BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 65536

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
                add_event(kq, fd, EVFILT_READ, EV_DELETE);
                return;
            }

            size_t new_size = buf_size * 2; // Double the buffer size
            char *new_buf = realloc(buf, new_size);
            if (!new_buf)
            {
                critical_logger("realloc failed");
                free(buf);
                add_event(kq, fd, EVFILT_READ, EV_DELETE);
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

    logger("Received data from %s: \n%s\n", INFO, client_ip, buf);

    // Add event to write response
    add_event(kq, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
}

void handle_response(int fd, int kq, const char *client_ip)
{
    ResultChar response = html_response("hello.html");
    if (response.ty == Ok)
    {
        // ssize_t sent_bytes = send(fd, response.val.res, strlen(response.val.res), 0);
        // Here can happen than the buffer is full and we need to wait for the socket to be ready to write
        send(fd, response.val.res, strlen(response.val.res), 0);
    }
    else
    {
        logger("%s %s", ERROR, e_to_string(&response.val.err), "Error getting html_response.");
    }

    free_result_char(&response);

    logger("Closing connection from %s", INFO, client_ip);

    // Remove filters
    add_event(kq, fd, EVFILT_WRITE, EV_DELETE);
    add_event(kq, fd, EVFILT_READ, EV_DELETE);
    // Close the file descriptor
    close(fd);
}
