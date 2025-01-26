#ifndef HANDLER_H
#define HANDLER_H

typedef struct PendingResponse
{
    int fd;
    char *data;
    char *start;
    int active;
    struct PendingResponse *next;
} PendingResponse;

void insert_new_connection(int fd, char *data, char *start);
PendingResponse *find_connection(int fd);
void add_event(int kq, int fd, int filter, int flags);
void handle_request(struct kevent event, int fd, int kq, const char *client_ip);
void handle_response(int fd, int kq, const char *client_ip);
#endif
