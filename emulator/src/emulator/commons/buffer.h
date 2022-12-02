#pragma once

#include <stddef.h>
#include <stdint.h>

struct buffer {
    uint8_t* data;
    size_t capacity;
};

struct buffer buffer_empty(void);
struct buffer buffer_new(size_t capacity);
void buffer_free(struct buffer* buffer);
