#pragma once

#include <stddef.h>
#include <string.h>

struct str_slice {
    const char* ptr;
    size_t len;
};

struct str_slice str_slice_empty(void);
struct str_slice str_slice_from_cstr_trusted(const char* cstr);
struct str_slice str_slice_from_cstr_untrusted(const char* cstr, size_t max_size);
struct str_slice str_slice_from_buffer(const char* buf, size_t buf_len);

