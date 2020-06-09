#include "thread_context.h"

void thread_context_init(thread_context* ctx, size_t tid, const char* name) {
    ctx->tid = tid;
    snprintf(ctx->name, THREADS_CTX_NAME_BUFFER_SIZE - 1, "Thread %lu", tid);
}
