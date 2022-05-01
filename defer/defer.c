#include "defer.h"

#include <assert.h>

void _defer_scope_begin(struct defer* defer)
{
    defer->len = 0;
}

void _defer_push_no_arg(struct defer* defer, void (*func)(void))
{
    assert(defer->len < DEFER_ITEMS_MAX_SIZE);

    defer->items[defer->len] = (struct defer_item) {
        .kind = DEFER_ITEM_KIND_NO_ARG,
        .as = {
            .item_no_arg = (struct defer_item_no_arg) {
                .func = func,
            }
        },
    };
    defer->len++;
}

void _defer_push_single_ptr_arg(struct defer* defer, void (*func)(void*), void* arg)
{
    assert(defer->len < DEFER_ITEMS_MAX_SIZE);

    defer->items[defer->len] = (struct defer_item) {
        .kind = DEFER_ITEM_KIND_SINGLE_PTR_ARG,
        .as = {
            .item_single_ptr_arg = (struct defer_item_single_ptr_arg) {
                .func = func,
                .arg = arg,
            },
        }
    };
    defer->len++;
}

void _defer_scope_end(struct defer* defer)
{
    for (int i = defer->len - 1; i >= 0; i--) {
        struct defer_item* item = &defer->items[i];

        switch (item->kind)
        {
        case DEFER_ITEM_KIND_NO_ARG:
            item->as.item_no_arg.func();
            break;
        
        case DEFER_ITEM_KIND_SINGLE_PTR_ARG: {
            struct defer_item_single_ptr_arg* i = &item->as.item_single_ptr_arg;
            i->func(i->arg);
        } break;

        default:
            assert(0 && "unknown defer item kind");
        }
    }
    defer->len = 0;
}
