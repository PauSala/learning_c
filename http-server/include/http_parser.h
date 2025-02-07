#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

#define PRINT_REQUEST "DEBUG_C_SERVER"

// clang -Wall -Wextra -pedantic -std=c99 main.c -o main
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

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
    UNKNOWN_METHOD,
    NO_METHOD
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
    "",
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
    case NO_METHOD:
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
    HEADER,
    NO_HEADERS,
    HEADER_END,
    BODY,
    PARSER_ERROR,
    END_PARSE
} ParserState;

const char *parser_state_to_string(ParserState state)
{
    switch (state)
    {
    case START:
        return "START";
    case METHOD:
        return "METHOD";
    case URL:
        return "URL";
    case VERSION:
        return "VERSION";
    case HEADERS:
        return "HEADERS";
    case HEADER:
        return "HEADER";
    case NO_HEADERS:
        return "NO_HEADERS";
    case HEADER_END:
        return "HEADER_END";
    case BODY:
        return "BODY";
    case PARSER_ERROR:
        return "PARSER_ERROR";
    case END_PARSE:
        return "END_PARSE";
    default:
        return "UNKNOWN";
    }
}

typedef struct
{
    ParserState state;
    bool eof;
    size_t start;
    size_t curr;
    HttpRequest *request;
    size_t input_len;
    char *data;

} HttpParser;

// Public declarations
int parse_request(HttpParser *parser);
void http_request_to_string(HttpParser *parser);
HttpParser *init_parser(char *req);
void free_parser(HttpParser *parser);
void append_request(HttpParser *parser, char *data);
HttpHeader *find_header(const char *key, HttpParser *parser);

// private
bool eor(HttpParser *parser);
void advance(HttpParser *parser);
bool expect_white_space(HttpParser *parser, char *req);

// Implementations
HttpParser *init_parser(char *buf)
{
    Span body = {.start = 0, .len = 0};
    Span url = {.start = 0, .len = 0};
    Span version = {.start = 0, .len = 0};
    HttpHeadersArray *headers = malloc(sizeof(HttpHeadersArray));
    if (!headers)
    {
        perror("malloc");
        free(buf);
        return NULL;
    }
    headers->capacity = 0;
    headers->length = 0;
    headers->headers = NULL;

    HttpRequest *req = malloc(sizeof(HttpRequest));
    if (!req)
    {
        perror("malloc");
        free(buf);
        return NULL;
    }
    req->headers = headers;
    req->body = body;
    req->url = url;
    req->version = version;

    HttpParser *parser = malloc(sizeof(HttpParser));
    if (!parser)
    {
        perror("malloc");
        free(buf);
        free(req);
        return NULL;
    }
    parser->input_len = strlen(buf);
    parser->state = START;
    parser->start = 0;
    parser->curr = 0;
    parser->request = req;
    parser->data = buf;
    parser->eof = false;

    return parser;
}

void free_parser(HttpParser *parser)
{
    if (parser)
    {
        if (parser->request)
        {
            if (parser->request->headers)
            {
                free(parser->request->headers);
            }
            free(parser->request);
        }
        if (parser->data)
        {
            free(parser->data);
        }
        free(parser);
    }
}

void append_request(HttpParser *parser, char *data)
{
    size_t old_len = parser->data ? strlen(parser->data) : 0;
    size_t new_len = old_len + strlen(data);

    char *new_buffer = realloc(parser->data, new_len + 1);
    if (!new_buffer)
    {
        perror("realloc in append_request");
        return;
    }

    strcpy(new_buffer + old_len, data);
    parser->data = new_buffer;
}

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

HttpHeader *find_header(const char *key, HttpParser *parser)
{
    if (!parser->request->headers)
    {
        return NULL;
    }
    for (int i = 0; i < parser->request->headers->length; i++)
    {
        HttpHeader *header = &parser->request->headers->headers[i];
        char *header_key = parser->data + header->key.start;
        size_t key_len = header->key.len;

        if (strncmp(header_key, key, key_len) == 0 && strlen(key) == key_len)
        {
            return header;
        }
    }
    return NULL;
}

long content_len(HttpParser *parser)
{
    HttpHeader *header = find_header("content-length", parser);
    if (header)
    {

        char *value_str = strndup(parser->data + header->value.start, header->value.len);
        if (value_str)
        {
            long content_length = strtol(value_str, NULL, 10);
            free(value_str);

            if (content_length > 0)
            {
                return content_length;
            }
        }
    }
    return -1;
}

void http_request_to_string(HttpParser *parser)
{

    char *req = parser->data;
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
    // size_t body_len = parser->request->body.len < 10 ? 10 : parser->request->body.len;
    printf("Body:\n%.*s\n", (int)(parser->request->body.len), req + parser->request->body.start);
}

// Checks if next character is SP. If EOF parser is marked as eof.
// If not EOF and not whitespace, parser transitions to PARSER_ERROR.
// If SP, cursor is advanced to the next char.
bool expect_white_space(HttpParser *parser, char *req)
{
    if ((int)req[parser->curr] == 0)
    {
        parser->eof = true;
        return false;
    }
    if ((int)req[parser->curr] != SP)
    {
        parser->state = PARSER_ERROR;
        return false;
    }
    advance(parser);
    return true;
}

//* * HTTP/*.*
#define MIN_RLINE_LEN 12

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
    assert(parser->state == METHOD);

    HttpMethod method = get_method(req, parser->start);
    if (method == UNKNOWN_METHOD)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    parser->request->method = method;
    parser->start = method_len(method);
    parser->curr = parser->start;
    parser->state = URL;

    expect_white_space(parser, req);
    return 0;
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
        parser->eof = true;
        return false;
    }

    if ((int)req[parser->curr] != CR)
    {
        parser->state = PARSER_ERROR;
        return false;
    }
    advance(parser);
    if (eor(parser))
    {
        parser->eof = true;
        return false;
    }

    if ((int)req[parser->curr] != LF)
    {
        parser->state = PARSER_ERROR;
        return false;
    }
    advance(parser);
    return true;
}

bool expect_crfl(HttpParser *parser, char *req)
{
    if (eor(parser))
    {
        parser->eof = true;
        return false;
    }

    if ((int)req[parser->curr] != CR)
    {
        return false;
    }

    if ((int)req[parser->curr + 1] == 0)
    {
        parser->eof = true;
        return false;
    }

    if ((int)req[parser->curr + 1] != LF)
    {
        return false;
    }
    return true;
}

int parse_url(HttpParser *parser, char *req)
{
    assert(parser->state == URL);
    do
    {
        if (eor(parser))
        {
            parser->eof = true;
            return 1;
        }
        if (!EXPECT_CHARACTER(req[parser->curr]))
        {
            break;
        }
        advance_curr(parser);
    } while (true);

    if (parser->start == parser->curr)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    parser->request->url.start = parser->start;
    parser->request->url.len = parser->curr - parser->start;
    parser->start = parser->curr;
    parser->state = VERSION;
    expect_white_space(parser, req);
    return 0;
}

int parse_version(HttpParser *parser, char *req)
{
    assert(parser->state == VERSION);
    do
    {
        if (eor(parser))
        {
            parser->eof = true;
            return 1;
        }
        if (!EXPECT_CHARACTER(req[parser->curr]))
        {
            break;
        }
        advance_curr(parser);
    } while (true);

    if (parser->start == parser->curr)
    {
        parser->state = PARSER_ERROR;
        return 1;
    }

    parser->request->version.start = parser->start;
    parser->request->version.len = parser->curr - parser->start;
    parser->start = parser->curr;
    parser->state = HEADER;

    consume_crfl(parser, req);
    return 0;
}

int parse_body(HttpParser *parser, char *req)
{
    assert(parser->state == HEADER_END || parser->state == BODY);
    size_t initial_start = parser->request->body.start;
    do
    {
        if (eor(parser))
        {
            parser->eof = true;
            parser->state = END_PARSE;
            break;
        }
        if (!IS_CHAR(req[parser->curr]))
        {
            break;
        }
        advance_curr(parser);
    } while (true);

    if (parser->start == parser->curr)
    {
        parser->state = END_PARSE;
        return 1;
    }

    parser->request->body.start = initial_start != 0 ? initial_start : parser->start;
    parser->request->body.len = initial_start != 0 ? parser->curr - initial_start : parser->curr - parser->start;
    parser->start = parser->curr;

    long content_length = content_len(parser);

    if ((long)parser->request->body.len < content_length)
    {
        parser->state = BODY;
        parser->eof = true;
    }
    else
    {
        parser->state = END_PARSE;
    }

    return 0;
}

int parse_header(HttpParser *parser, char *req)
{
    if (expect_crfl(parser, req))
    {
        consume_crfl(parser, req);
        parser->state = HEADER_END;
        return 0;
    }

    size_t initial_start = parser->start;
    assert(parser->state == HEADER);
    if (eor(parser))
    {
        parser->eof = true;
        return 1;
    }

    while (IS_TOKEN(req[parser->curr]))
    {
        advance_curr(parser);
        if (eor(parser))
        {
            parser->eof = true;
            return 1;
        }
    }

    if (parser->start == parser->curr)
    {
        parser->state = NO_HEADERS;
        return 1;
    }
    Span key = {.start = parser->start, .len = parser->curr - parser->start};
    parser->start = parser->curr;
    if (req[parser->curr] != ':')
    {
        parser->eof = true;
        parser->start = initial_start;
        parser->curr = initial_start;
        return 1;
    }

    advance(parser);
    if (eor(parser))
    {
        parser->eof = true;
        parser->start = initial_start;
        parser->curr = initial_start;
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
        parser->eof = true;
        parser->start = initial_start;
        parser->curr = initial_start;
        return 1;
    }
    while (IS_VALUE(req[parser->curr]))
    {
        advance_curr(parser);
        if (eor(parser))
        {
            parser->eof = true;
            parser->start = initial_start;
            parser->curr = initial_start;
            return 1;
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

    consume_crfl(parser, req);

    return 0;
}

bool eor(HttpParser *parser)
{
    return parser->curr >= parser->input_len;
}

int parse_request(HttpParser *parser)
{

    while (parser->state != PARSER_ERROR && parser->state != END_PARSE && !parser->eof)
    {
        switch (parser->state)
        {
        case START:
            if (parser->input_len < MIN_RLINE_LEN)
            {
                parser->eof = true;
                break;
            }
            parser->state = METHOD;
            break;
        case METHOD:
            parse_method(parser, parser->data);
            break;
        case URL:
            parse_url(parser, parser->data);
            break;
        case VERSION:
            parse_version(parser, parser->data);
            break;
        case HEADER:
            parse_header(parser, parser->data);
            break;
        case HEADER_END:
            parse_body(parser, parser->data);
            break;
        case BODY:
            parse_body(parser, parser->data);
            break;
        default:
            break;
        }
    }

    return 0;
}

#endif // PARSE_REQUEST_H
