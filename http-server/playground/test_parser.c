// clang -Wall -Wextra -pedantic -std=c99 test_parser->c -o test
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "../include/http_parser.h"

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

void assert_span_equal(const char *base, size_t start, size_t len, const char *expected)
{
    assert(strncmp(base + start, expected, len) == 0);
    assert(strlen(expected) == len);
}

int main(void)
{
    char *request = read_request();

    HttpParser *parser = init_parser(request);

    if (!parser)
    {
        assert(1 == 2);
    }

    parse_request(parser);
    http_request_to_string(parser);

    assert(parser->request->method == GET);
    assert_span_equal(request, parser->request->url.start, parser->request->url.len, "/");
    assert_span_equal(request, parser->request->version.start, parser->request->version.len, "HTTP/1.1");

    HttpHeader *header = &parser->request->headers->headers[0];
    assert_span_equal(request, header->key.start, header->key.len, "accept-encoding");
    assert_span_equal(request, header->value.start, header->value.len, "gzip, deflate, br");

    HttpHeader *header2 = &parser->request->headers->headers[1];
    assert_span_equal(request, header2->key.start, header2->key.len, "Accept");
    assert_span_equal(request, header2->value.start, header2->value.len, "*/*");

    HttpHeader *header3 = &parser->request->headers->headers[2];
    assert_span_equal(request, header3->key.start, header3->key.len, "User-Agent");
    assert_span_equal(request, header3->value.start, header3->value.len, "Thunder Client (https://www.thunderclient.com)");

    HttpHeader *header4 = &parser->request->headers->headers[3];
    assert_span_equal(request, header4->key.start, header4->key.len, "Host");
    assert_span_equal(request, header4->value.start, header4->value.len, "localhost:3000");

    assert(parser->request->body.len == 44);

    assert(parser->state == PARSER_EOF);

    free_parser(parser);

    return 0;
}
