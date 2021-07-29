#ifndef NATURAL_PARSER_UTILS_H
#define NATURAL_PARSER_UTILS_H

#include "string-view.h"

void* panic_if_null(void* ptr);

#define ALLOC_OR_PANIC(T) \
    panic_if_null(malloc(sizeof(T)))

char* file_read_or_panic(const char* file_path);

StringView file_read_to_string_view_or_panic(const char* file_path);

#endif
