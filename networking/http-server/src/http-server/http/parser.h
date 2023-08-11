#pragma once

#include <regex.h>

#include <http-server/str.h>
#include <http-server/error.h>
#include "method.h"

struct http_parser {
    struct str_slice input;
    struct str_slice cursor;

    regex_t method_pattern;
};

void http_parser_init(struct http_parser* parser, struct str_slice input);
void http_parser_free(struct http_parser* parser);

enum result http_parser_method_parse(struct http_parser* parser, enum http_method* out);

