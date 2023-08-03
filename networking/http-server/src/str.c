#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

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

enum return_code parse_ull(const char* input, unsigned long long* output)
{
    assert(input != NULL);
    assert(output != NULL);

    char* endptr = NULL;
    errno = 0;
    unsigned long long value = strtoull(input, &endptr, 10);
    if (errno != 0 || value == ULLONG_MAX) {
        return RC_ERROR;
    }

    *output = value;
    return RC_OK;
}

