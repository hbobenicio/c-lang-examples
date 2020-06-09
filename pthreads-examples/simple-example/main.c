/**
 * Basic Threading Example.
 * 
 * @see https://computing.llnl.gov/tutorials/pthreads/
 */

// C Standard (LIBC)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// POSIX
#include <pthread.h>
#include <unistd.h>

#include "thread_context.h"

#define THREADS_SIZE 4

void panic(const char* msg);
void* on_thread_create(void *data);

int main() {
    pthread_t threads[THREADS_SIZE];
    thread_context threads_context[THREADS_SIZE];

    // 1) If a thread requires joining, consider explicitly creating it as joinable.
    // This provides portability as not all implementations may create threads as joinable by default. 
    //
    // 2) Since some implementations of Pthreads may not create threads in a joinable state,
    // the threads in this example are explicitly created in a joinable state so that they can be joined later.
    pthread_attr_t common_thread_attrs;
    pthread_attr_init(&common_thread_attrs);
    pthread_attr_setdetachstate(&common_thread_attrs, PTHREAD_CREATE_JOINABLE);

    puts("[info] starting threads...");

    for (size_t i = 0; i < THREADS_SIZE; i++) {
        // Thread context Initialization
        thread_context* ctx = &threads_context[i];

        ctx->tid = i;
        snprintf(ctx->name, THREADS_CTX_NAME_BUFFER_SIZE - 1, "Thread %lu", i);
        
        if (pthread_create(&threads[i], &common_thread_attrs, on_thread_create, (void*) ctx)) {
            panic("error: thread creation failed\n");
        }
    }

    pthread_attr_destroy(&common_thread_attrs);

    for (size_t i = 0; i < THREADS_SIZE; i++) {
        if (pthread_join(threads[i], NULL)) {
            panic("error: thread joining failed\n");
        }
    }

    puts("[info] finished!");
}

void panic(const char* msg) {
    fputs(msg, stderr);
    exit(EXIT_FAILURE);
}

void* on_thread_create(void *data) {
    thread_context* ctx = (thread_context*) data;

    printf("%s has started!\n", ctx->name);

    sleep(1);
    
    printf("%s has finished!\n", ctx->name);

    return NULL;
}
