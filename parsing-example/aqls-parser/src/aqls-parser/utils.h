#ifndef AQLS_PARSER_UTILS_H
#define AQLS_PARSER_UTILS_H

#include "string-view.h"

void* panic_if_null(void* ptr);

#define ALLOC_OR_PANIC(T) \
    panic_if_null(malloc(sizeof(T)))

char* file_read_or_panic(const char* file_path);

struct strview file_read_to_string_view_or_panic(const char* file_path);

#endif
