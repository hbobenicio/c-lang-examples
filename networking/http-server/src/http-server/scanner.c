#include "scanner.h"

#include <string.h>

enum result scanner_next_until_char(struct scanner* s, char c, struct str_slice* out)
{
    if (str_slice_is_empty(s->input)) {
        return RESULT_ERR;
    }

    const char* ptr = memchr(s->input.ptr, (int) c, s->input.len);
    if (ptr == NULL) {
        return RESULT_ERR;
    }

    size_t offset = ptr - s->input.ptr;

    out->len = offset;
    out->ptr = s->input.ptr;

    s->input.ptr = ptr;
    s->input.len -= offset;

    return RESULT_OK;
}

enum result scanner_next_until_slice(struct scanner* s, struct str_slice pattern, struct str_slice* out)
{
    if (str_slice_is_empty(s->input)) {
        return RESULT_ERR;
    }

    const char* ptr = memmem(s->input.ptr, s->input.len, pattern.ptr, pattern.len);
    if (ptr == NULL) {
        return RESULT_ERR;
    }

    size_t offset = ptr - s->input.ptr;

    out->len = offset;
    out->ptr = s->input.ptr;

    s->input.ptr = ptr;
    s->input.len -= offset;

    return RESULT_OK;
}

enum result scanner_next_until_end(struct scanner* s, struct str_slice* out)
{
    if (str_slice_is_empty(s->input)) {
        return RESULT_ERR;
    }

    *out = s->input;
    s->input.ptr += s->input.len;
    s->input.len -= s->input.len;
    return RESULT_OK;
}

