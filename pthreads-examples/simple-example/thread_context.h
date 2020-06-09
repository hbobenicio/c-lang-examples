#ifndef THREAD_CONTEXT_H
#define THREAD_CONTEXT_H

#include <stdlib.h>

#define THREADS_CTX_NAME_BUFFER_SIZE 50

/**
 * Thread Context (Data).
 * Each thread will get a reference for it's own object.
 */
typedef struct {
    size_t tid;
    char name[THREADS_CTX_NAME_BUFFER_SIZE];
} thread_context;

void thread_context_init(thread_context* ctx, size_t tid, const char* name);

#endif
