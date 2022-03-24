#include "string-view.h"

#include <string.h>

struct strview string_view_empty()
{
    struct strview sv = {0};
    return sv;
}

struct strview string_view_from_cstr(const char *str)
{
    return string_view_from_sized_cstr(str, strlen(str));
}

struct strview string_view_from_sized_cstr(const char* str, size_t str_len)
{
    return (struct strview) {
        .str = str,
        .len = str_len,
    };
}

size_t string_view_fwrite(struct strview sv, FILE* stream)
{
    if (sv.str != NULL && sv.len > 0) {
        return fwrite(sv.str, sizeof(char), sv.len, stream);
    }
    return 0;
}

int string_view_cmp(struct strview a, struct strview b)
{
    return memcmp(a.str, b.str, (a.len < b.len) ? a.len : b.len);
}

int string_view_cmp_cstr(struct strview a, const char* b)
{
    return string_view_cmp_sized_cstr(a, b, strlen(b));
}

int string_view_cmp_sized_cstr(struct strview a, const char* b, size_t b_len)
{
    return string_view_cmp(a, string_view_from_sized_cstr(b, b_len));
}
