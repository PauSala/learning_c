#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "../include/logger.h"

char *interpolate(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    size_t message_size = vsnprintf(NULL, 0, format, args) + 1;
    char *message = (char *)malloc(message_size);
    if (!message)
    {
        perror("malloc failed");
        exit(1);
    }

    va_end(args);
    va_start(args, format);
    // Format the message
    vsnprintf(message, message_size, format, args);

    va_end(args);
    return message;
}

// Logging function that formats and logs messages
void logger(const char *format, LogLevel level, ...)
{
    const char *level_str = NULL;

    switch (level)
    {
    case ERROR:
        level_str = RED "[ERROR] " RESET;
        break;
    case WARN:
        level_str = YELLOW "[WARN] " RESET;
        break;
    case DEBUG:
        level_str = MAGENTA "[DEBUG] " RESET;
        break;
    case INFO:
    default:
        level_str = CYAN "[INFO] " RESET;
        break;
    }

    va_list args;
    va_start(args, level);

    // Calculate the buffer size needed for the formatted message
    size_t message_size = strlen(level_str) + vsnprintf(NULL, 0, format, args) + 1;

    // Allocate memory for the full message
    char *message = (char *)malloc(message_size);
    if (!message)
    {
        perror("malloc failed");
        exit(1);
    }

    // Re-initialize va_list to use it again in vsnprintf
    va_end(args);
    va_start(args, level);

    // Format the message
    vsnprintf(message, message_size, format, args);
    printf("%s%s\n", level_str, message);

    va_end(args);
    free(message);
}
