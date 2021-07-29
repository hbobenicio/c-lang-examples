#include "queue.h"

#include <stdlib.h>
#include <assert.h>

void natural_token_queue_init(NaturalTokenQueue* queue, size_t capacity)
{
    queue->items = calloc(capacity, sizeof(NaturalToken));
    if (queue->items == NULL) {
        fputs("error: natural: token: queue: out of memory.\n", stderr);
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->len = queue->start = queue->end = 0;
}

void natural_token_queue_free(NaturalTokenQueue* queue)
{
    free(queue->items);
}

void natural_token_queue_append(NaturalTokenQueue* queue, NaturalToken item)
{
    assert(!natural_token_queue_is_full(queue));
    
    queue->items[queue->end] = item;
    queue->end = (queue->end + 1) % queue->capacity;
    queue->len++;
}

NaturalToken natural_token_queue_pop(NaturalTokenQueue* queue)
{
    assert(!natural_token_queue_is_empty(queue));

    NaturalToken token = queue->items[queue->start];
    queue->start = (queue->start + 1) % queue->capacity;
    queue->len--;

    return token;
}

NaturalToken natural_token_queue_peek(const NaturalTokenQueue* queue)
{
    assert(!natural_token_queue_is_empty(queue));

    return queue->items[queue->start];
}

bool natural_token_queue_is_empty(const NaturalTokenQueue* queue)
{
    return queue->len == 0;
}

bool natural_token_queue_is_full(const NaturalTokenQueue* queue)
{
    return queue->len == queue->capacity;
}
