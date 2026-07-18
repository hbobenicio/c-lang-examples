#include "scgi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>

static size_t scgi_parser_cursor_size (const struct scgi_parser* parser);
static size_t scgi_parser_total_parsed(const struct scgi_parser* parser);
static bool   scgi_parser_eof         (const struct scgi_parser* parser);

void scgi_parser_init(struct scgi_parser* parser, const char* input_buffer, size_t input_buffer_size)
{
    assert(parser != NULL);
    assert(input_buffer != NULL);
    assert(input_buffer_size > 0);

    parser->state = SCGI_PARSER_STATE_PARSING_HEADER_NAME;
    parser->input_buffer = parser->cursor_start = parser->cursor_end = input_buffer;
    parser->input_buffer_size = input_buffer_size;
}

int scgi_parser_parse(struct scgi_parser* parser)
{
    assert(parser != NULL);
    fprintf(stderr, "info: scgi: parsing...\n");

    // Example of a request
    // const char scgi_request[] =
    //     "CONTENT_LENGTH\0"
    //     "13\0"
    //     "SCGI\0"
    //     "1\0"
    //     ","
    //     "Hello, world!";

    // parsing result
    size_t res_msg_size = 0;

    while (parser->state != SCGI_PARSER_STATE_DONE && !scgi_parser_eof(parser)) {
        switch (parser->state) {
            // case SCGI_PARSER_STATE_PARSING_MSG_LEN: {
            //     if (isdigit(*parser->cursor_end)) {
            //         parser->cursor_end++;
            //         continue;
            //     }
            //     if (*parser->cursor_end != ':') {
            //         fprintf(stderr, "error: scgi parsing failed: colon ':' not found\n");
            //         return 1;
            //     }
            //     fprintf(stderr, "debug: scgi parsing: ':' found\n");
            //     // ':' found!
            //     char number[32] = {0};
            //     size_t number_byte_len = scgi_parser_cursor_size(parser);
            //     if (number_byte_len > 9) {
            //         //TODO improve this logic...
            //         fprintf(stderr, "error: scgi parsing failed: message size has too many digits\n");
            //         return 1;
            //     }
            //     memcpy(number, parser->cursor_start, number_byte_len);
            //     res_msg_size = atoi(number);
            //     parser->state = SCGI_PARSER_STATE_PARSING_HEADER_NAME;
            //     parser->cursor_end++; // skip ':'
            //     parser->cursor_start = parser->cursor_end;
            //     fprintf(stderr, "debug: scgi parsing: got msg size: %zu\n", res_msg_size);
            // } break;

            case SCGI_PARSER_STATE_PARSING_HEADER_NAME: {
                if (*parser->cursor_end == ',') {
                    parser->state = SCGI_PARSER_STATE_PARSING_BODY;
                    parser->cursor_end++; // skip ','
                    parser->cursor_start = parser->cursor_end;
                    continue;
                }
                if (*parser->cursor_end != '\0') {
                    parser->cursor_end++;
                    continue;
                }
                // '\0' found!
                char header_name[256] = {0};
                size_t header_name_byte_len = scgi_parser_cursor_size(parser);
                if (header_name_byte_len >= sizeof(header_name) - 1) {
                    //TODO improve this logic...
                    fprintf(stderr, "error: scgi parsing failed: header name has too many bytes/chars\n");
                    return 1;
                }
                memcpy(header_name, parser->cursor_start, header_name_byte_len);
                parser->state = SCGI_PARSER_STATE_PARSING_HEADER_VALUE;
                parser->cursor_end++; // skip '\0'
                parser->cursor_start = parser->cursor_end;
                fprintf(stderr, "debug: scgi parsing: got header name: \"%s\"\n", header_name);
            } break;

            case SCGI_PARSER_STATE_PARSING_HEADER_VALUE: {
                if (*parser->cursor_end != '\0') {
                    parser->cursor_end++;
                    continue;
                }
                // '\0' found!
                char header_value[256] = {0};
                size_t header_value_byte_len = scgi_parser_cursor_size(parser);
                if (header_value_byte_len >= sizeof(header_value) - 1) {
                    //TODO improve this logic...
                    fprintf(stderr, "error: scgi parsing failed: header value has too many bytes/chars\n");
                    return 1;
                }
                memcpy(header_value, parser->cursor_start, header_value_byte_len);
                parser->state = SCGI_PARSER_STATE_PARSING_HEADER_NAME;
                parser->cursor_end++; // skip '\0'
                parser->cursor_start = parser->cursor_end;
                fprintf(stderr, "debug: scgi parsing: got header value: \"%s\"\n", header_value);
            } break;

            case SCGI_PARSER_STATE_PARSING_BODY: {
                // read until eof
                parser->cursor_end++;
            } break;
            
            // case SCGI_PARSER_STATE_DONE: {

            // } break;
            default:
                fprintf(stderr, "error: scgi parsing: unexpected parsing state. state=%d\n", parser->state);
                return -1;
        }
    }
    if (scgi_parser_eof(parser) && parser->state == SCGI_PARSER_STATE_PARSING_BODY) {
        char body[256] = {0};
        size_t body_byte_len = scgi_parser_cursor_size(parser);
        if (body_byte_len >= sizeof(body) - 1) {
            //TODO improve this logic...
            fprintf(stderr, "error: scgi parsing failed: body has too many bytes/chars\n");
            return 1;
        }
        memcpy(body, parser->cursor_start, body_byte_len);
        parser->state = SCGI_PARSER_STATE_DONE;
        parser->cursor_start = parser->cursor_end;
        fprintf(stderr, "debug: scgi parsing: got body: \"%s\"\n", body);
        return 0;
    }
    fprintf(stderr, "error: scgi parsing: illegal state\n");
    return 1;
}

static size_t scgi_parser_total_parsed(const struct scgi_parser* parser)
{
    return parser->cursor_start - parser->input_buffer;
}

static bool scgi_parser_eof(const struct scgi_parser* parser)
{
    // return scgi_parser_total_parsed(parser) > parser->input_buffer_size;
    return parser->cursor_end >= (parser->input_buffer + parser->input_buffer_size);
}

static size_t scgi_parser_cursor_size(const struct scgi_parser* parser)
{
    return parser->cursor_end - parser->cursor_start;
}

int scgi_send_text(const struct scgi_connection* conn, int status_code, const char* text, size_t text_len)
{
    assert(conn != NULL);
    assert(status_code >= 100 && status_code < 600);
    assert(text != NULL);
    assert(text_len > 0);

    char output_buffer[SCGI_OUTPUT_BUFFER_SIZE] = {0};

    if (text_len > SCGI_OUTPUT_BUFFER_SIZE) {
        fprintf(stderr,
            "error: output buffer overflow. output_buffer_size=%d text_len=%zu\n",
            SCGI_OUTPUT_BUFFER_SIZE, text_len
        );
        return 1;
    }

    int bytes_formatted = snprintf(output_buffer, SCGI_OUTPUT_BUFFER_SIZE,
        "Status: %d OK\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "\r\n"
        "%s",
        status_code, text_len, text
    );
    fprintf(stderr, "debug: %d bytes formatted to the output buffer\n", bytes_formatted);
    assert(bytes_formatted > 0);
    assert((size_t) bytes_formatted < SCGI_OUTPUT_BUFFER_SIZE);
    
    fprintf(stderr, "debug: sending output buffer:\n\"%s\"\n", output_buffer);
    ssize_t bytes_sent = send(conn->client_socket_fd, output_buffer, bytes_formatted, 0);
    fprintf(stderr, "debug: send(): bytes_sent=%ld\n", bytes_sent);
    if (bytes_sent == -1) {
        int error_code = errno;
        const char* err_cause = strerror(error_code);
        fprintf(stderr, "error: send(): failed: %s\n", err_cause);
        return 1;
    }

    return 0;
}
