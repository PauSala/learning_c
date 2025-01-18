
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/html_res.h"
#include "../include/logger.h"
#include "../include/response_t.h"

#define OK_200 "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n%s"
#define TEMPLATE_PATH "templates"
#define MAX_PATH_LENGTH 1024
#define PATH_TOO_LONG "Combined path is too long"
#define READ_HTML_IDENTIFIER "read_html_file -> "

ResultChar read_html_file(const char *file_path)
{

    static char full_path[MAX_PATH_LENGTH];
    size_t required_length = strlen(TEMPLATE_PATH) + strlen(file_path) + 1;
    if (required_length >= MAX_PATH_LENGTH)
    {
        char *err = PATH_TOO_LONG;
        char *full = interpolate("%s %s: %s", READ_HTML_IDENTIFIER, err, file_path);
        return result_char(Err, full);
    }

    snprintf(full_path, sizeof(full_path), "%s/%s", TEMPLATE_PATH, file_path);

    FILE *file = fopen(full_path, "r");
    if (!file)
    {
        char *err = strdup(strerror(errno));
        char *full = interpolate("%s %s: %s", READ_HTML_IDENTIFIER, err, full_path);
        free(err);
        return result_char(Err, full);
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer for the file content
    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer)
    {
        char *err = strdup(strerror(errno));
        char *full = interpolate("%s %s", READ_HTML_IDENTIFIER, err);
        free(err);
        return result_char(Err, full);
    }

    // Read the file into the buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    return result_char(Ok, buffer);
}

ResultChar html_response(const char *template_path)
{
    ResultChar html_response = read_html_file(template_path);
    if (html_response.ty == Err)
    {
        return html_response;
    }

    int content_length = strlen(html_response.val.res);

    // Format the response with the HTML content
    char *response = (char *)malloc(strlen(OK_200) + content_length + 1);
    if (!response)
    {
        char *err = strdup(strerror(errno));
        char *full = interpolate("%s %s", READ_HTML_IDENTIFIER, err);
        free(err);
        return result_char(Err, full);
    }

    sprintf(response, OK_200, content_length, html_response.val.res);
    return result_char(Ok, response);
}
