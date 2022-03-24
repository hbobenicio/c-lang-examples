#include "queue.h"

#include <stdlib.h>
#include <assert.h>

void aqls_token_queue_init(struct aqls_token_queue* queue, size_t capacity)
{
    queue->items = calloc(capacity, sizeof(struct aqls_token));
    if (queue->items == NULL) {
        fputs("error: aqls: token: queue: out of memory.\n", stderr);
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->len = queue->start = queue->end = 0;
}

void aqls_token_queue_free(struct aqls_token_queue* queue)
{
    free(queue->items);
}

void aqls_token_queue_append(struct aqls_token_queue* queue, struct aqls_token item)
{
    assert(!aqls_token_queue_is_full(queue));
    
    queue->items[queue->end] = item;
    queue->end = (queue->end + 1) % queue->capacity;
    queue->len++;
}

struct aqls_token aqls_token_queue_pop(struct aqls_token_queue* queue)
{
    assert(!aqls_token_queue_is_empty(queue));

    struct aqls_token token = queue->items[queue->start];
    queue->start = (queue->start + 1) % queue->capacity;
    queue->len--;

    return token;
}

struct aqls_token aqls_token_queue_peek(const struct aqls_token_queue* queue)
{
    assert(!aqls_token_queue_is_empty(queue));

    return queue->items[queue->start];
}

bool aqls_token_queue_is_empty(const struct aqls_token_queue* queue)
{
    return queue->len == 0;
}

bool aqls_token_queue_is_full(const struct aqls_token_queue* queue)
{
    return queue->len == queue->capacity;
}
