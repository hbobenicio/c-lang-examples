#include "queue.h"

#include <stdlib.h>
#include <assert.h>

void aqls_token_queue_init(AqlsTokenQueue* queue, size_t capacity)
{
    queue->items = calloc(capacity, sizeof(AqlsToken));
    if (queue->items == NULL) {
        fputs("error: aqls: token: queue: out of memory.\n", stderr);
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->len = queue->start = queue->end = 0;
}

void aqls_token_queue_free(AqlsTokenQueue* queue)
{
    free(queue->items);
}

void aqls_token_queue_append(AqlsTokenQueue* queue, AqlsToken item)
{
    assert(!aqls_token_queue_is_full(queue));
    
    queue->items[queue->end] = item;
    queue->end = (queue->end + 1) % queue->capacity;
    queue->len++;
}

AqlsToken aqls_token_queue_pop(AqlsTokenQueue* queue)
{
    assert(!aqls_token_queue_is_empty(queue));

    AqlsToken token = queue->items[queue->start];
    queue->start = (queue->start + 1) % queue->capacity;
    queue->len--;

    return token;
}

AqlsToken aqls_token_queue_peek(const AqlsTokenQueue* queue)
{
    assert(!aqls_token_queue_is_empty(queue));

    return queue->items[queue->start];
}

bool aqls_token_queue_is_empty(const AqlsTokenQueue* queue)
{
    return queue->len == 0;
}

bool aqls_token_queue_is_full(const AqlsTokenQueue* queue)
{
    return queue->len == queue->capacity;
}
