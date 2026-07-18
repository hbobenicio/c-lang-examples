#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#include "net.h"

struct slice slice_create(void* ptr, size_t len)
{
    return (struct slice) {
        .ptr = ptr,
        .len = len,
    };
}

void buffered_reader_init(struct buffered_reader* reader, int fd, void* buffer, size_t buffer_size)
{
    assert(reader != NULL);
    assert(buffer != NULL);
    assert(buffer_size > 0);

    *reader = (struct buffered_reader) {
        .input_fd = fd,
        .buffer = slice_create(buffer, buffer_size),
        .cursor = buffer,
    };
}

int buffered_reader_read_digits_until(struct buffered_reader* reader, char delim, size_t max_digits)
{
    assert(!isdigit(delim));

    size_t digits_count = 0;
    while (true) {
        if (reader->cursor >= (char*)reader->buffer.ptr + reader->buffer.len) {
            fprintf(stderr, "debug: io: buffer overflow at \"%s:%d\"\n", __FILE__, __LINE__);
            return 1;
        }
        char c;
        if (recv_char(reader->input_fd, &c) <= 0) {
            return 1;
        }
        
        if (c == delim) {
            fprintf(stderr, "debug: io: done reading digits. digits_count=%zu delim='%c'\n", digits_count, delim);
            break;
        }

        if (!isdigit(c)) {
            fprintf(stderr, "error: non digit received before delimiter. got=%d digits_count=%zu delim='%c'\n", c, digits_count, delim);
            return 1;
        }

        if (digits_count >= max_digits) {
            fprintf(stderr, "error: too much digits. max_digits=%zu\n", max_digits);
            return 1;
        }

        *reader->cursor = c;
        reader->cursor++;
    }

    return 0;
}

struct fixed_buffer_allocator fixed_buffer_allocator_from_buffer(void* buffer, size_t buffer_size)
{
    assert(buffer != NULL);
    assert(buffer_size > 0);

    return (struct fixed_buffer_allocator) {
        .buffer = slice_create(buffer, buffer_size),
        .allocated = 0,
    };
}

struct fixed_buffer_allocator fixed_buffer_allocator_from_slice(struct slice slice)
{
    return (struct fixed_buffer_allocator) {
        .buffer = slice,
        .allocated = 0,
    };
}

void* fixed_buffer_allocator_malloc(struct fixed_buffer_allocator* allocator, size_t size)
{
    assert(allocator != NULL);
    assert(size > 0);

    if (allocator->allocated + size > allocator->buffer.len) {
        return NULL;
    }

    void* ptr = (char*) allocator->buffer.ptr + allocator->allocated;
    allocator->allocated += size;
    assert(allocator->allocated <= allocator->buffer.len);

    return ptr;
}

void* fixed_buffer_allocator_calloc(struct fixed_buffer_allocator* allocator, size_t size)
{
    void* ptr = fixed_buffer_allocator_malloc(allocator, size);
    if (ptr == NULL) {
        return NULL;
    }

    return memset(ptr, 0, size);
}
