#ifndef HANDLER_H
#define HANDLER_H
void handle_client(int fd, int kq, const char *client_ip);
void add_event(int kq, int fd, int filter, int flags);
#endif
