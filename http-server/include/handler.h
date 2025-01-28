#ifndef HANDLER_H
#define HANDLER_H

typedef struct PartialWrite
{
    size_t bytes_sent;
} PartialWrite;

void add_event(int kq, int fd, int filter, int flags);
void handle_request(struct kevent *event, int fd, int kq, const char *client_ip);
void handle_response(struct kevent *event, int fd, int kq, const char *client_ip);
#endif
