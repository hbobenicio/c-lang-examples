#include "http.h"

#include <stdio.h>
#include <assert.h>

struct http_header_iter http_header_iter_from_headers(struct str_slice headers)
{
    return (struct http_header_iter) {
        .headers_buffer = headers,
        .header = {0},
        .done = false,
    };
}

struct http_header_iter http_header_next(struct http_header_iter iter)
{
    const char* buf = iter.headers_buffer.ptr;
    size_t buf_len = iter.headers_buffer.len;

    int state = 0;
    for (const char* end = buf; *end != '\0'; end++) {
        size_t offset = (size_t) (end - buf);
        if (offset >= buf_len) {
            break;
        }

        char c = *end;

        if (state == 0 && c == '\r') {
            state = 1;
            continue;
        }
        if (state == 1 && c == '\n') {
            state = 2;
            continue;
        }
        if (state == 2) {
            //FIXME refactor state machine so that it includes the end of the headers with \r\n\r\n (and skip whitespaces)
            struct http_header_iter header = {
                .headers_buffer = {
                    .ptr = buf + offset,
                    .len = buf_len - offset,
                },
                .header = {
                    .key = str_slice_empty(), //TODO parse it!   (offset - 2 is the total length)
                    .value = str_slice_empty(), //TODO parse it!
                },
                .done = false,
            };
            return header;
        }
        state = 0;
    }
    return (struct http_header_iter) {
        .headers_buffer = {0},
        .header = {0},
        .done = true,
    };
}

