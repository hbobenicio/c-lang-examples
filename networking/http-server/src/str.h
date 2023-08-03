#pragma once

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"

struct str_slice {
    const char* ptr;
    size_t len;
};

struct str_slice str_slice_empty(void);
struct str_slice str_slice_from_cstr_trusted(const char* cstr);
struct str_slice str_slice_from_cstr_untrusted(const char* cstr, size_t max_size);
struct str_slice str_slice_from_buffer(const char* buf, size_t buf_len);

bool str_slice_is_empty(struct str_slice s);

// Parsing Utilities
enum return_code parse_ull(const char* input, unsigned long long* output);

