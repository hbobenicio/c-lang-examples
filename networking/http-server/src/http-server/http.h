#pragma once

#include "error.h"
#include "str.h"

#include "dynamic-array.h"

#define HTTP_REQUEST_METHOD_BUFFER_SIZE 8
#define HTTP_REQUEST_METHOD_MAX_SIZE HTTP_REQUEST_METHOD_BUFFER_SIZE - 1  // leave room for the trailing '\0'

#define HTTP_REQUEST_URL_BUFFER_SIZE 64
#define HTTP_REQUEST_URL_MAX_SIZE HTTP_REQUEST_URL_BUFFER_SIZE - 1  // leave room for the trailing '\0'

#define HTTP_REQUEST_PROTOCOL_BUFFER_SIZE 16
#define HTTP_REQUEST_PROTOCOL_MAX_SIZE HTTP_REQUEST_PROTOCOL_BUFFER_SIZE - 1

#define HTTP_REQUEST_HEADERS_BUFFER_SIZE 64

#define HTTP_REQUEST_BODY_BUFFER_SIZE 4096

/**
 * @brief a simple http header
 */
struct http_header {
    struct str_slice key;
    struct str_slice value;
};

struct http_request_start_line {
    struct str_slice method;
    struct str_slice path;
    struct str_slice version;
};

/**
 * @brief a simple http request
 */
struct http_request {

    /**
     * @brief the raw unsafe request message data read from the connection socket.
     */
    struct dynamic_array data;

    /**
     * @brief the client connection socket file descriptor
     */
    int conn;

    struct http_request_start_line start_line;

    struct http_header headers[HTTP_REQUEST_HEADERS_BUFFER_SIZE];

    struct str_slice body;
};

enum result http_headers_validate(uint8_t* unsafe_cstr_buffer,
                                  size_t unsafe_cstr_buffer_size,
                                  size_t *out_end_of_headers_offset);

/**
 * @deprecated
 */
enum result http_request_poll_and_read(struct http_request* req);

enum result http_method_parse(struct str_slice input, struct str_slice* out);

