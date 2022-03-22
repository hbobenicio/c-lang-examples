#ifndef AQLS_PARSER_STRING_VIEW_H
#define AQLS_PARSER_STRING_VIEW_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    const char* str;
    size_t len;
} StringView;

StringView string_view_empty();
StringView string_view_from_cstr(const char* str);
StringView string_view_from_sized_cstr(const char* str, size_t str_len);

size_t string_view_fwrite(StringView sv, FILE* stream);
int string_view_cmp(StringView a, StringView b);
int string_view_cmp_cstr(StringView a, const char* b);
int string_view_cmp_sized_cstr(StringView a, const char* b, size_t b_len);

#endif
