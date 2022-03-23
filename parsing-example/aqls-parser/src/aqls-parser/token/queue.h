#ifndef AQLS_PARSER_TOKEN_QUEUE_H
#define AQLS_PARSER_TOKEN_QUEUE_H

#include <stddef.h>
#include <stdbool.h>

#include "token.h"

struct aqls_token_queue {
    AqlsToken* items;

    size_t start;
    size_t end;

    size_t capacity;
    size_t len;
};

void aqls_token_queue_init(struct aqls_token_queue* queue, size_t capacity);
void aqls_token_queue_free(struct aqls_token_queue* queue);

void aqls_token_queue_append(struct aqls_token_queue* queue, AqlsToken item);
AqlsToken aqls_token_queue_pop(struct aqls_token_queue* queue);
AqlsToken aqls_token_queue_peek(const struct aqls_token_queue* queue);

bool aqls_token_queue_is_empty(const struct aqls_token_queue* queue);
bool aqls_token_queue_is_full(const struct aqls_token_queue* queue);

#endif
