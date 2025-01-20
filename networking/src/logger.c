#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "../include/logger.h"

#define BUFFER_SIZE 1024

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
        level_str = YELLOW "[WARN]  " RESET;
        break;
    case DEBUG:
        level_str = MAGENTA "[DEBUG] " RESET;
        break;
    case INFO:
    default:
        level_str = CYAN "[INFO]  " RESET;
        break;
    }

    char message[BUFFER_SIZE];
    va_list args;
    va_start(args, level);

    int written = vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    if (written < 0 || (size_t)written >= sizeof(message))
    {
        fprintf(stderr, YELLOW "[WARN]  Log message truncated\n");
    }

    printf("%s%s\n", level_str, message);
}

void critical_logger(const char *message)
{
    fprintf(stderr, BG_BRIGHT_RED BLACK "[CRITICAL ERROR] %s" RESET "\n", message);
}

char *interpolate(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    size_t message_size = vsnprintf(NULL, 0, format, args) + 1;
    char *message = (char *)malloc(message_size);
    if (!message)
    {
        perror("Logger error: malloc failed");
        exit(1);
    }

    va_end(args);
    va_start(args, format);
    // Format the message
    vsnprintf(message, message_size, format, args);

    va_end(args);
    return message;
}
