#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

// clang -Wall -Wextra -pedantic -std=c99 main.c -o main
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define SP 32
#define HT 9
#define CR 13
#define LF 10

#define IS_SEPARATOR(c)                                                     \
    ((c) == '(' || (c) == ')' || (c) == '<' || (c) == '>' || (c) == '@' ||  \
     (c) == ',' || (c) == ';' || (c) == ':' || (c) == '\\' || (c) == '"' || \
     (c) == '/' || (c) == '[' || (c) == ']' || (c) == '?' || (c) == '=' ||  \
     (c) == '{' || (c) == '}' || (int)(c) == SP || (int)(c) == HT)

#define IS_CHAR(c) ((int)(c) >= 0 && (int)(c) <= 127)

#define IS_VALUE(c) \
    (IS_CHAR(c) && !IS_CTL(c))

#define IS_CTL(c) \
    (((int)(c) >= 0 && (int)(c) <= 31) || (int)(c) == 127)

#define IS_TOKEN(c) \
    (IS_CHAR(c) && !IS_CTL(c) && !IS_SEPARATOR(c))

#define EXPECT_CHARACTER(c) \
    ((int)(c) != 0 &&       \
     (int)(c) != SP &&      \
     (int)(c) != HT &&      \
     (int)(c) != CR &&      \
     (int)(c) != LF)

typedef enum
{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
    UNKNOWN_METHOD
} HttpMethod;

static const char *HTTP_METHODS[] = {
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "CONNECT",
    "OPTIONS",
    "TRACE",
    "PATCH",
    "UNKNOWN_METHOD",
};

const char *method_to_string(HttpMethod method)
{
    return HTTP_METHODS[method];
}

size_t method_len(HttpMethod method)
{
    switch (method)
    {
    case GET:
        return 3;
        break;
    case HEAD:
        return 4;
        break;
    case POST:
        return 4;
        break;
    case PUT:
        return 3;
        break;
    case DELETE:
        return 6;
        break;
    case CONNECT:
        return 7;
        break;
    case OPTIONS:
        return 7;
        break;
    case TRACE:
        return 5;
        break;
    case PATCH:
        return 5;
        break;
    case UNKNOWN_METHOD:
        return 0;
        break;
    }
}

typedef struct
{
    size_t start;
    size_t len;
} Span;

typedef struct
{
    Span key;
    Span value;
} HttpHeader;

typedef struct
{
    HttpHeader *headers;
    int32_t length;
    int32_t capacity;
} HttpHeadersArray;

typedef struct
{
    HttpMethod method;
    Span url;
    Span version;
    HttpHeadersArray *headers;
    Span body;
} HttpRequest;

typedef enum
{
    START,
    METHOD,
    URL,
    VERSION,
    HEADERS,
    HEADER_K,
    HEADER_V,
    BODY,
    DONE,
    PARSER_ERROR,
} ParserState;

typedef struct
{
    ParserState state;
    size_t start;
    size_t curr;
    HttpRequest *request;
    size_t input_len;

} HttpParser;

// Public declarations
int parse_request(HttpParser *parser, char *req);
void http_request_to_string(HttpParser *parser, char *req);

// private
bool eor(HttpParser *parser);

// Implementations
static int resize_http_headers_array(HttpHeadersArray *array)
{
    int new_capacity = array->capacity * 2;
    if (new_capacity == 0)
    {
        new_capacity = 1;
    }

    HttpHeader *new_headers = realloc(array->headers, new_capacity * sizeof(HttpHeader));
    if (new_headers == NULL)
    {
        return -1;
    }

    array->headers = new_headers;
    array->capacity = new_capacity;
    return 0;
}

int insert_header(HttpHeadersArray *array, Span key, Span value)
{
    if (array->length >= array->capacity)
    {
        if (resize_http_headers_array(array) != 0)
        {
            return -1;
        }
    }

    HttpHeader *new_header = &array->headers[array->length];
    new_header->key = key;
    new_header->value = value;

    array->length++;

    return 0;
}

void http_request_to_string(HttpParser *parser, char *req)
{
    printf("Method: %s\n", method_to_string(parser->request->method));
    printf("URL: %.*s\n", (int)(parser->request->url.len), req + parser->request->url.start);
    printf("Version: %.*s\n", (int)(parser->request->version.len), req + parser->request->version.start);
    for (int i = 0; i < parser->request->headers->length; i++)
    {
        HttpHeader *header = &parser->request->headers->headers[i];
        printf("Header: %.*s:%.*s\n",
               (int)(header->key.len), req + header->key.start,
               (int)(header->value.len), req + header->value.start);
    }
    printf("Body:\n%.*s\n", (int)(parser->request->body.len), req + parser->request->body.start);
}

#define MAX_METHOD_LEN 7

// Helper function to match methods
HttpMethod get_method(const char *req, size_t index)
{

    if (req[index] == 'G')
    {
        if (strncmp(req, "GET", 3) == 0)
            return GET;
        else
            return UNKNOWN_METHOD;
    }
    else if (req[index] == 'H')
    {
        if (strncmp(req, "HEAD", 4) == 0)
            return HEAD;
        else
            return UNKNOWN_METHOD;
    }
    else if (req[index] == 'P')
    {
        if (req[index + 1] == 'O' && strncmp(req, "POST", 4) == 0)
        {
            return POST;
        }
        if (req[index + 1] == 'U' && strncmp(req, "PUT", 3) == 0)
        {
            return PUT;
        }
        if (req[index + 1] == 'A' && strncmp(req, "PATCH", 5) == 0)
        {
            return PATCH;
        }
        return UNKNOWN_METHOD;
    }
    else if (req[index] == 'D')
    {
        if (strncmp(req, "DELETE", 6) == 0)
            return DELETE;
        else
            return UNKNOWN_METHOD;
    }
    else if (req[index] == 'C')
    {
        if (strncmp(req, "CONNECT", 7) == 0)
            return CONNECT;
        else
            return UNKNOWN_METHOD;
    }
    else if (req[index] == 'O')
    {
        if (strncmp(req, "OPTIONS", 7) == 0)
            return OPTIONS;
        else
            return UNKNOWN_METHOD;
    }
    else if (req[index] == 'T')
    {
        if (strncmp(req, "TRACE", 5) == 0)
            return TRACE;
        else
            return UNKNOWN_METHOD;
    }

    return UNKNOWN_METHOD;
}

int parse_method(HttpParser *parser, char *req)
{
    if (parser->state != START)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    if (parser->input_len < MAX_METHOD_LEN)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    HttpMethod method = get_method(req, parser->start);
    if (method == UNKNOWN_METHOD)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    parser->request->method = method;
    parser->start = method_len(method);
    parser->curr = parser->start;
    parser->state = METHOD;

    return 0;
}

bool expect_white_space(HttpParser *parser, char *req)
{
    if ((int)req[parser->curr] == 0)
    {
        return false;
    }
    if ((int)req[parser->curr] == SP)
    {
        return true;
    }
    return false;
}

void advance(HttpParser *parser)
{
    parser->start++;
    parser->curr = parser->start;
}

void advance_curr(HttpParser *parser)
{
    parser->curr++;
}

bool consume_crfl(HttpParser *parser, char *req)
{
    if (eor(parser))
    {
        parser->state = PARSER_ERROR;
        return false;
    }

    if ((int)req[parser->curr] != CR)
    {
        return false;
    }
    advance(parser);
    if (eor(parser))
    {
        parser->state = PARSER_ERROR;
        return false;
    }
    if ((int)req[parser->curr] != LF)
    {
        return false;
    }
    advance(parser);
    if (eor(parser))
    {
        parser->state = PARSER_ERROR;
        return false;
    }
    return true;
}

int parse_url(HttpParser *parser, char *req)
{
    while (EXPECT_CHARACTER(req[parser->curr]))
    {
        advance_curr(parser);
        if (parser->curr > parser->input_len)
        {
            break;
        }
    }

    if (parser->start == parser->curr)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    parser->request->url.start = parser->start;
    parser->request->url.len = parser->curr - parser->start;
    parser->start = parser->curr;
    parser->state = URL;
    return 0;
}

int parse_version(HttpParser *parser, char *req)
{
    while (EXPECT_CHARACTER(req[parser->curr]))
    {
        advance_curr(parser);
        if (eor(parser))
        {
            parser->state = PARSER_ERROR;
            break;
        }
    }

    if (parser->start == parser->curr)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    parser->request->version.start = parser->start;
    parser->request->version.len = parser->curr - parser->start;
    parser->start = parser->curr;
    parser->state = VERSION;
    return 0;
}

int parse_body(HttpParser *parser, char *req)
{
    while (IS_CHAR(req[parser->curr]))
    {
        advance_curr(parser);
        if (eor(parser))
        {
            break;
        }
    }

    if (parser->start == parser->curr)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    parser->request->body.start = parser->start;
    parser->request->body.len = parser->curr - parser->start;
    parser->start = parser->curr;
    parser->state = BODY;
    return 0;
}

int parse_header(HttpParser *parser, char *req)
{
    while (IS_TOKEN(req[parser->curr]))
    {
        advance_curr(parser);
        if (eor(parser))
        {
            parser->state = PARSER_ERROR;
            break;
        }
    }

    if (parser->start == parser->curr)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }
    Span key = {.start = parser->start, .len = parser->curr - parser->start};
    parser->start = parser->curr;
    if (req[parser->curr] != ':')
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    advance(parser);
    if (eor(parser))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }
    if (req[parser->curr] != SP)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    advance(parser);
    if (eor(parser))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }
    while (IS_VALUE(req[parser->curr]))
    {
        advance_curr(parser);
        if (eor(parser))
        {
            parser->state = PARSER_ERROR;
            break;
        }
    }

    if (parser->start == parser->curr)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }
    Span value = {.start = parser->start, .len = parser->curr - parser->start};
    insert_header(parser->request->headers, key, value);
    parser->start = parser->curr;
    return 0;
}

bool eor(HttpParser *parser)
{
    return parser->curr >= parser->input_len;
}

int parse_request(HttpParser *parser, char *req)
{
    // TODO: start parsing from current state!

    // Parse method
    parse_method(parser, req);
    if (!expect_white_space(parser, req))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }
    advance(parser);
    if (eor(parser))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    // Parse URL
    parse_url(parser, req);
    if (!expect_white_space(parser, req))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }
    advance(parser);
    if (eor(parser))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    // Parse Version
    parse_version(parser, req);
    if (!consume_crfl(parser, req))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    while (!consume_crfl(parser, req))
    {
        parser->state = HEADERS;
        parse_header(parser, req);
        if (!consume_crfl(parser, req))
        {
            parser->state = PARSER_ERROR;
            return 1;
        }
    };

    if (consume_crfl(parser, req))
    {
        parser->state = PARSER_ERROR;
        return 1;
    }
    // Parse body
    parse_body(parser, req);

    return 0;
}

#endif
