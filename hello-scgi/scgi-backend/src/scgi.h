#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "mem.h"

#define SCGI_INPUT_BUFFER_SIZE   4096
#define SCGI_OUTPUT_BUFFER_SIZE  4096
#define SCGI_READ_CHUNK_SIZE      100
#define SCGI_HEADERS_MAX_SIZE    1024
#define SCGI_REQ_MAX_BUFFER_SIZE 4096

struct scgi_connection {
    int client_socket_fd;
};

struct scgi_request {
    char input_buffer[SCGI_INPUT_BUFFER_SIZE];
};

struct scgi_response_writer {
    int status_code;
    char output_buffer[SCGI_OUTPUT_BUFFER_SIZE];
};

struct scgi_context {
    struct scgi_connection conn;
    struct scgi_request req;
    struct scgi_response_writer resp;
    struct fixed_buffer_allocator allocator;
};

enum scgi_parser_state {
    // SCGI_PARSER_STATE_PARSING_MSG_LEN = 0,
    SCGI_PARSER_STATE_PARSING_HEADER_NAME = 0,
    SCGI_PARSER_STATE_PARSING_HEADER_VALUE,
    SCGI_PARSER_STATE_PARSING_BODY,
    SCGI_PARSER_STATE_DONE,
};

struct scgi_parser {
    enum scgi_parser_state state;

    const char* input_buffer;
    size_t input_buffer_size;

    const char* cursor_start;
    const char* cursor_end;
};

void scgi_parser_init (struct scgi_parser* parser, const char* input_buffer, size_t input_buffer_size);
int  scgi_parser_parse(struct scgi_parser* parser);
int scgi_send_text(const struct scgi_connection* conn, int status_code, const char* text, size_t text_len);
