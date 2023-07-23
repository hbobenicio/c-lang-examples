///////////////////////////
// Interface Declaration //
///////////////////////////
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define DYNAMIC_ARRAY_INITIAL_CAPACITY 8
#define DYNAMIC_ARRAY_GROW_FACTOR      2

/**
 * uint8_t dynamic array
 */
struct dynamic_array {

    /**
     * Buffer that will hold all the items
     */
    uint8_t* items;

    /**
     * Number of inserted items
     */
    size_t length;

    /**
     * Total items buffer capacity in bytes
     */
    size_t capacity;
};

void dynamic_array_init(struct dynamic_array* self);
 int dynamic_array_init_with_capacity(struct dynamic_array* self, size_t initial_capacity);
void dynamic_array_free(struct dynamic_array* self);
bool dynamic_array_is_full(const struct dynamic_array* self);
 int dynamic_array_push_back(struct dynamic_array* self, uint8_t item);
 int dynamic_array_ensure_capacity(struct dynamic_array* self, size_t required_capacity);

#endif // DYNAMIC_ARRAY_H

////////////////////////////////
// Implementation Definitions //
////////////////////////////////
// #define DYNAMIC_ARRAY_IMPLEMENTATION
#ifdef DYNAMIC_ARRAY_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void dynamic_array_init(struct dynamic_array* self)
{
    assert(self != NULL);

    *self = (struct dynamic_array) {
        .items = NULL,
        .capacity = 0,
        .length = 0,
    };
}

int dynamic_array_init_with_capacity(struct dynamic_array* self, size_t initial_capacity)
{
    assert(self != NULL);
    assert(initial_capacity > 0);

    void* buffer = calloc(1, initial_capacity);
    if (buffer == NULL) {
        return 1;
    }

    *self = (struct dynamic_array) {
        .items = (uint8_t*) buffer,
        .capacity = initial_capacity,
        .length = 0,
    };

    return 0;
}

void dynamic_array_free(struct dynamic_array* self)
{
    assert(self != NULL);

    if (self->items) {
        free(self->items);
        self->items = NULL;
    }
    self->capacity = 0;
    self->length = 0;
}

bool dynamic_array_is_full(const struct dynamic_array* self)
{
    assert(self);
    return self->length >= self->capacity;
}

int dynamic_array_push_back(struct dynamic_array* self, uint8_t item)
{
    assert(self != NULL);

    if (dynamic_array_is_full(self)) {
        size_t new_capacity = (self->capacity == 0)
                            ? DYNAMIC_ARRAY_INITIAL_CAPACITY
                            : DYNAMIC_ARRAY_GROW_FACTOR * self->capacity;
        self->items = (uint8_t*) realloc(self->items, new_capacity);
        if (self->items == NULL) {
            return 1;
        }
        self->capacity = new_capacity;
    }
    self->items[self->length] = item;
    self->length++;
    return 0;
}

int dynamic_array_ensure_capacity(struct dynamic_array* self, size_t required_capacity)
{
    assert(self);
    if (required_capacity <= self->capacity) {
        return 0;
    }
    //TODO continue...
    return 1;
}

#endif

