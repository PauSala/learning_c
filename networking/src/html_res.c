
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/html_res.h"
#include "../include/logger.h"
#include "../include/response_t.h"
#include "../include/errors.h"

#define OK_200 "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n%s"
#define TEMPLATE_PATH "templates"
#define MAX_PATH_LENGTH 1024
#define PATH_TOO_LONG "Combined path is too long"
#define READ_HTML_IDENTIFIER "read_html_file -> "
#define MERROR "FAILED TO ALLOCATE"

static char *cached_html = NULL;

ResultChar read_html_file(const char *file_path)
{

    if (cached_html != NULL)
    {
        return result_char(Ok, cached_html);
    }

    static char full_path[MAX_PATH_LENGTH];
    size_t required_length = strlen(TEMPLATE_PATH) + strlen(file_path) + 1;
    if (required_length >= MAX_PATH_LENGTH)
    {
        char *err = PATH_TOO_LONG;
        logger("%s %s: %d", DEBUG, READ_HTML_IDENTIFIER, err, required_length);
        return result_char(Err, PATH_TO_LONG_E);
    }

    snprintf(full_path, sizeof(full_path), "%s/%s", TEMPLATE_PATH, file_path);

    FILE *file = fopen(full_path, "r");
    if (!file)
    {
        char *err = strdup(strerror(errno));
        logger("%s %s: %s", DEBUG, READ_HTML_IDENTIFIER, err, full_path);
        free(err);
        return result_char(Err, IO_E);
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer for the file content
    cached_html = (char *)malloc(file_size + 1);
    if (!cached_html)
    {
        critical_logger("Failed to allocate");
        return result_char(Err, MEMORY_E);
    }

    // Read the file into the buffer
    fread(cached_html, 1, file_size, file);
    cached_html[file_size] = '\0';

    fclose(file);
    return result_char(Ok, cached_html);
}

static char *cached_res = NULL;

ResultChar html_response(const char *template_path)
{

    if (cached_res != NULL)
    {
        logger("Cached response", ERROR);
        return result_char(Ok, cached_res);
    }

    ResultChar html_response = read_html_file(template_path);
    if (html_response.ty == Err)
    {
        return html_response;
    }

    // TODO: This is wrong
    int content_length = strlen(html_response.val.res) + 1;

    // Format the response with the HTML content
    int len = strlen(OK_200) + content_length + 1;
    cached_res = (char *)malloc(len);
    if (!cached_res)
    {
        critical_logger("Failed to allocate");
        return result_char(Err, MEMORY_E);
    }

    snprintf(cached_res, len, OK_200, content_length, html_response.val.res);
    // Do not free the html since it is cached, this should be refactored for sure
    // Since having to know this fact here is horribly wrong
    return result_char(Ok, cached_res);
}
