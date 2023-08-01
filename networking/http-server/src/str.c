#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

struct str_slice str_slice_empty(void)
{
    return (struct str_slice) {
        .ptr = NULL,
        .len = 0,
    };
}

struct str_slice str_slice_from_cstr_trusted(const char* cstr)
{
    return (struct str_slice) {
        .ptr = cstr,
        .len = strlen(cstr),
    };
}

struct str_slice str_slice_from_cstr_untrusted(const char* cstr, size_t max_len)
{
    return (struct str_slice) {
        .ptr = cstr,
        .len = strnlen(cstr, max_len),
    };
}

struct str_slice str_slice_from_buffer(const char* buf, size_t buf_len)
{
    return (struct str_slice) {
        .ptr = buf,
        .len = buf_len,
    };
}

bool str_slice_is_empty(struct str_slice s)
{
    return s.ptr == NULL || s.len == 0;
}

int parse_ul(const char* input, unsigned long* output)
{
    char* endptr = NULL;
    errno = 0;
    unsigned long value = strtoul(input, &endptr, 10);
    if (errno != 0 || value >= ULONG_MAX) {
        return 1;
    }

    *output = value;
    return 0;
}

