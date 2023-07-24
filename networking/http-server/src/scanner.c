#include "scanner.h"

#include <string.h>

int scanner_next_until_char(struct scanner* s, char c, struct str_slice* out)
{
    if (str_slice_is_empty(s->input)) {
        return 1;
    }

    const char* ptr = memchr(s->input.ptr, (int) c, s->input.len);
    if (ptr == NULL) {
        return 1;
    }
    
    size_t offset = ptr - s->input.ptr;
    s->input.ptr = ptr + 1;
    s->input.len -= offset + 1;

    out->ptr = ptr;
    out->len = offset;

    return 0;
}

int scanner_next_until_slice(struct scanner* s, struct str_slice pattern, struct str_slice* out)
{
    if (str_slice_is_empty(s->input)) {
        return 1;
    }

    const char* ptr = memmem(s->input.ptr, s->input.len, pattern.ptr, pattern.len);
    if (ptr == NULL) {
        return 1;
    }

    size_t offset = ptr - s->input.ptr;
    s->input.ptr = ptr + 1;
    s->input.len -= offset + 1;

    out->ptr = ptr;
    out->len = offset;

    return 0;
}

int scanner_next_until_end(struct scanner* s, struct str_slice* out)
{
    if (str_slice_is_empty(s->input)) {
        return 1;
    }

    *out = s->input;
    s->input.ptr += s->input.len;
    s->input.len -= s->input.len;
    return 0;
}

