// clang -Wall -Wextra -pedantic -std=c99 test_parser.c -o main
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../include/parse_request.h"

char *read_request(void)
{
    FILE *file = fopen("request.txt", "r");
    if (!file)
    {
        printf("File read error");
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *request = (char *)malloc(file_size + 1);
    if (!request)
    {
        printf("malloc read error");
    }

    // Read the file into the buffer
    fread(request, 1, file_size, file);
    fclose(file);
    return request;
}

int main(void)
{
    char *request = read_request();

    Span body = {.start = 0, .len = 0};
    Span url = {.start = 0, .len = 0};
    Span version = {.start = 0, .len = 0};
    HttpHeadersArray headers = {NULL, 0, 0};
    HttpRequest req = {.headers = &headers, .body = body, .url = url, .version = version};
    HttpParser parser = {
        .input_len = strlen(request),
        .state = START,
        .start = 0,
        .curr = 0,
        .request = &req};

    parse_request(&parser, request);
    http_request_to_string(&parser, request);

    return 0;
}
