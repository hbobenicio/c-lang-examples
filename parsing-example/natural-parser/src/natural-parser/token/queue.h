#ifndef NATURAL_PARSER_TOKEN_QUEUE_H
#define NATURAL_PARSER_TOKEN_QUEUE_H

#include <stddef.h>
#include <stdbool.h>

#include "token.h"

typedef struct {
    NaturalToken* items;

    size_t start;
    size_t end;

    size_t capacity;
    size_t len;
} NaturalTokenQueue;

void natural_token_queue_init(NaturalTokenQueue* queue, size_t capacity);
void natural_token_queue_free(NaturalTokenQueue* queue);

void natural_token_queue_append(NaturalTokenQueue* queue, NaturalToken item);
NaturalToken natural_token_queue_pop(NaturalTokenQueue* queue);
NaturalToken natural_token_queue_peek(const NaturalTokenQueue* queue);

bool natural_token_queue_is_empty(const NaturalTokenQueue* queue);
bool natural_token_queue_is_full(const NaturalTokenQueue* queue);

#endif
