#include "http.h"

#include <stdio.h>
#include <assert.h>

#include "scanner.h"

void http_request_init(struct http_request* req, struct str_slice input)
{
    assert(req != NULL);
    req->input = input;
    int rc = dynamic_array_init_with_capacity(&req->headers, 4098);
    assert(rc == 0);
}

int http_request_parse(struct http_request* req)
{
    fprintf(stderr, "debug: http: request: parsing...\n");

    int rc = http_request_headers_parse(req);
    if (rc != 0) {
        fprintf(stderr, "error: http: request: parsing failed.\n");
        return 1;
    }

    fprintf(stderr, "debug: http: request: parsing success.\n");
    return 0;
}

int http_request_headers_parse(struct http_request* req)
{
    fprintf(stderr, "debug: http: request: headers: parsing...\n");

    // while not "\r\n\r\n":
    //     try header_parse()
    int rc = http_request_header_parse(req);
    if (rc != 0) {
        fprintf(stderr, "debug: http: request: headers: parsing failed.\n");
        return 1;
    }

    fprintf(stderr, "debug: http: request: headers: parsing success.\n");
    return 0;
}

int http_request_header_parse(struct http_request* req)
{
    fprintf(stderr, "debug: http: request: header: parsing...\n");

    struct scanner s = { .input = req->input };
    struct str_slice eol = str_slice_from_cstr_trusted("\r\n");
    struct str_slice header_slice;

    int rc = scanner_next_until_slice(&s, eol, &header_slice);
    if (rc != 0) {
        fprintf(stderr, "debug: http: request: header: parsing failed.\n");
        return 1;
    }

    //FIXME parse header key and value correctly
    struct http_header header = {
        .key = header_slice,
        .value = header_slice,
    };

    rc = dynamic_array_push_back(&req->headers, (const uint8_t*) &header, sizeof(header));
    assert(rc == 0);

    return 0;
}

