#include "../include/errors.h"

char *e_to_string(ServerError *e)
{
    switch (*e)
    {
    case IO_E:
        return "IO_ERROR";
    case MEMORY_E:
        return "ALLOCATION ERROR";
    case PATH_TO_LONG_E:
        return "PATH_TO_LONG";
    default:
        return "UNKNOWN ERROR";
    };
}
