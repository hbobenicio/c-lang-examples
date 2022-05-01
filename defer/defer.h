#ifndef DEFER_H
#define DEFER_H

// #include <stdarg.h>

/// PUBLIC

#ifndef DEFER_ITEMS_MAX_SIZE
#define DEFER_ITEMS_MAX_SIZE 16
#endif

#define defer_scope_begin() \
    struct defer defer; \
    _defer_scope_begin(&defer);

#define defer_push_0 defer_push_no_arg
#define defer_push_no_arg(func) \
    _defer_push_no_arg(&defer, func);

#define defer_push_1 defer_push_single_ptr_arg
#define defer_push_single_ptr_arg(func, arg) \
    _defer_push_single_ptr_arg(&defer, func, arg);

#define defer_scope_end() \
    _defer_scope_end(&defer);

/// PRIVATE

enum defer_item_kind {
    DEFER_ITEM_KIND_NO_ARG,
    DEFER_ITEM_KIND_SINGLE_PTR_ARG,
};

struct defer_item_no_arg {
    void (*func)(void);
};

struct defer_item_single_ptr_arg {
    void (*func)(void*);
    void* arg;
};

struct defer_item {
    enum defer_item_kind kind;
    union {
        struct defer_item_no_arg item_no_arg;
        struct defer_item_single_ptr_arg item_single_ptr_arg;
    } as;
};

struct defer {
    struct defer_item items[DEFER_ITEMS_MAX_SIZE];
    int len;
};

void _defer_scope_begin(struct defer* defer);
void _defer_push_no_arg(struct defer* defer, void (*func)(void));
void _defer_push_single_ptr_arg(struct defer* defer, void (*func)(void*), void* arg);
void _defer_scope_end(struct defer* defer);

#endif
