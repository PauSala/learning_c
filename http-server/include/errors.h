#ifndef ERRORS_H
#define ERRORS_H

typedef enum
{
    IO_E,
    MEMORY_E,
    PATH_TO_LONG_E

} ServerError;

char *e_to_string(ServerError *e);

#endif
