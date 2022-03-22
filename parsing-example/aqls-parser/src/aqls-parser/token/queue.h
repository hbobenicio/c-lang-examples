#ifndef AQLS_PARSER_TOKEN_QUEUE_H
#define AQLS_PARSER_TOKEN_QUEUE_H

#include <stddef.h>
#include <stdbool.h>

#include "token.h"

typedef struct {
    AqlsToken* items;

    size_t start;
    size_t end;

    size_t capacity;
    size_t len;
} AqlsTokenQueue;

void aqls_token_queue_init(AqlsTokenQueue* queue, size_t capacity);
void aqls_token_queue_free(AqlsTokenQueue* queue);

void aqls_token_queue_append(AqlsTokenQueue* queue, AqlsToken item);
AqlsToken aqls_token_queue_pop(AqlsTokenQueue* queue);
AqlsToken aqls_token_queue_peek(const AqlsTokenQueue* queue);

bool aqls_token_queue_is_empty(const AqlsTokenQueue* queue);
bool aqls_token_queue_is_full(const AqlsTokenQueue* queue);

#endif
