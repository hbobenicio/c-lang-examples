#pragma once

#include <stddef.h>

#define ARRAY_SIZE(XS) (sizeof(XS) / sizeof((XS)[0]))

struct slice {
    void* ptr;
    size_t len;
};

struct slice slice_create(void* ptr, size_t len);

struct buffered_reader {
    int input_fd;
    struct slice buffer;
    char* cursor;
};

void buffered_reader_init(struct buffered_reader* reader, int fd, void* buffer, size_t buffer_size);
int  buffered_reader_read_digits_until(struct buffered_reader* reader, char delim, size_t max_digits);

struct fixed_buffer_allocator {
    struct slice buffer;
    size_t allocated;
};

struct fixed_buffer_allocator fixed_buffer_allocator_from_buffer(void* buffer, size_t buffer_size);
struct fixed_buffer_allocator fixed_buffer_allocator_from_slice(struct slice slice);
void* fixed_buffer_allocator_malloc(struct fixed_buffer_allocator* allocator, size_t size);
void* fixed_buffer_allocator_calloc(struct fixed_buffer_allocator* allocator, size_t size);
