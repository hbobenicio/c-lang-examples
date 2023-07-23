#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "str.h"

struct http_header {
    struct str_slice key;
    struct str_slice value;
};

struct http_header_iter {
    struct str_slice headers_buffer;
    struct http_header header;
    bool done;
};

struct http_header_iter http_header_iter_from_buffer(struct str_slice headers_buffer);
struct http_header_iter http_header_next(struct http_header_iter iter);


