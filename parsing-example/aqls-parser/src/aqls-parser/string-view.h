#ifndef AQLS_PARSER_STRING_VIEW_H
#define AQLS_PARSER_STRING_VIEW_H

#include <stddef.h>
#include <stdio.h>

struct strview {
    const char* str;
    size_t len;
};

struct strview string_view_empty();
struct strview string_view_from_cstr(const char* str);
struct strview string_view_from_sized_cstr(const char* str, size_t str_len);

size_t string_view_fwrite(struct strview sv, FILE* stream);
int string_view_cmp(struct strview a, struct strview b);
int string_view_cmp_cstr(struct strview a, const char* b);
int string_view_cmp_sized_cstr(struct strview a, const char* b, size_t b_len);

#endif
