#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

#include "../include/logger.h"
#include "../include/response_t.h"
#include "../include/html_res.h"

#define INITIAL_BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 65536

// TODO: this is not working at all
void handle_client(int fd, const char *client_ip)
{
    size_t buf_size = INITIAL_BUFFER_SIZE;
    char *buf = malloc(buf_size);
    if (!buf)
    {
        critical_logger("failed to allocate");
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
                return;
            }

            size_t new_size = buf_size * 2; // Double the buffer size
            char *new_buf = realloc(buf, new_size);
            if (!new_buf)
            {
                critical_logger("realloc failed");
                free(buf);
                return;
            }

            buf = new_buf;
            buf_size = new_size;
        }
    }

    if (nbytes == 0)
    {
        /// logger("Client disconnected: %s", INFO, client_ip);
        return;
    }
    else if (nbytes == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        char *err = strdup(strerror(errno));
        logger("recv: %s", ERROR, err);
        free(err);
        return;
    }

    logger("Received data from %s: \n%s\n", INFO, client_ip, buf);

    ResultChar response = html_response("hello.html");
    if (response.ty == Ok)
    {
        send(fd, response.val.res, strlen(response.val.res), 0);
    }
    else
    {
        logger("%s %s", ERROR, e_to_string(&response.val.err), "Error getting html_response.");
    }

    free_result_char(&response);

    free(buf);
}
