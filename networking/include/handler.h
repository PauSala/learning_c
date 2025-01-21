#ifndef HANDLER_H
#define HANDLER_H
void add_event(int kq, int fd, int filter, int flags);
void handle_request(int fd, int kq, const char *client_ip);
void handle_response(int fd, int kq, const char *client_ip);
#endif
