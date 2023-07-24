#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "dynamic-array.h"
#include "str.h"

struct http_header {
    struct str_slice key;
    struct str_slice value;
};

struct http_request {
    /**
     * raw unsafe input data read from the connection socket. This should be the owned data where all slices and other
     * borrows should reference too.
     */ 
    struct str_slice input;
    struct dynamic_array headers;
};

void http_request_init(struct http_request* req, struct str_slice input);
int http_request_parse(struct http_request* req);
int http_request_headers_parse(struct http_request* req);
int http_request_header_parse(struct http_request* req);

