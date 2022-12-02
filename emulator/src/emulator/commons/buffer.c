#include "buffer.h"

#include <stdlib.h>
#include <assert.h>

struct buffer buffer_empty(void)
{
    return (struct buffer) {
        .data = NULL,
        .capacity = 0,
    };
}

struct buffer buffer_new(size_t capacity)
{
    uint8_t* data = calloc(1, capacity);
    assert(data != NULL);

    return (struct buffer) {
        .data = data,
        .capacity = capacity,
    };
}

void buffer_free(struct buffer* buffer)
{
    free(buffer->data);
    buffer->capacity = 0;
    buffer->data = NULL;
}
