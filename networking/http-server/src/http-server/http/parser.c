#include "parser.h"

#include <stdlib.h>
#include <assert.h>

#include <http-server/commons.h>
#include <http-server/scanner.h>

void http_parser_init(struct http_parser* parser, struct str_slice input)
{
    assert(parser != NULL);
    assert(input.ptr != NULL);
    parser->input = input;
    parser->cursor = input;
    regex_must_compile(&parser->method_pattern, "^GET|PUT|POST|HEAD|TRACE|DELETE|CONNECT|OPTIONS", REG_EXTENDED);
}

void http_parser_free(struct http_parser* parser)
{
    assert(parser != NULL);
    regfree(&parser->method_pattern);
}

enum result http_parser_method_parse(struct http_parser* parser, enum http_method* out)
{
    assert(parser != NULL);
    assert(parser->cursor.ptr != NULL);
    assert(out != NULL);
    return RESULT_ERR;
}

/*
enum result http_parser_method_parse(struct http_parser* parser, struct str_slice* out)
{
    assert(parser != NULL);
    assert(parser->cursor.ptr != NULL);

    if (parser->cursor.len < 3 || parser->cursor.len > 8) {
        return RESULT_ERR;
    }

#define RETURN_IF_MATCHES(M) \
    do { \
        const char* ptr = memmem(parser->cursor.ptr, parser->cursor.len, M, sizeof(M) - 1); \
        if (ptr != NULL) { \
            *out = (struct str_slice) { \
                .ptr = parser->cursor.ptr, \
                .len = ptr - parser->cursor.ptr, \
            }; \
            return RESULT_OK; \
        } \
    } while (false)

    RETURN_IF_MATCHES("GET");
    RETURN_IF_MATCHES("PUT");
    RETURN_IF_MATCHES("POST");
    RETURN_IF_MATCHES("HEAD");
    RETURN_IF_MATCHES("TRACE");
    RETURN_IF_MATCHES("DELETE");
    RETURN_IF_MATCHES("CONNECT");
    RETURN_IF_MATCHES("OPTIONS");

#undef RETURN_IF_METHOD_MATCHES

    return RESULT_ERR;
}
*/
