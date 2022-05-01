#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defer.h"

int gotoless_duplicated_approach(void);
int goto_approach(void);

int defer_gotoless_approach(void);

int main()
{
    gotoless_duplicated_approach();
    goto_approach();

    defer_gotoless_approach();
    return 0;
}

int gotoless_duplicated_approach(void)
{
    int* x1 = malloc(1 * sizeof(int));
    if (!x1) {
        perror("oom");
        return 1;
    }

    int* x2 = malloc(1 * sizeof(int));
    if (!x2) {
        perror("oom");
        free(x1);
        return 1;
    }

    int* x3 = malloc(1 * sizeof(int));
    if (!x3) {
        perror("oom");
        free(x2);
        free(x1);
        return 1;
    }

    int* x4 = malloc(1 * sizeof(int));
    if (!x4) {
        perror("oom");
        free(x3);
        free(x2);
        free(x1);
        return 1;
    }

    free(x4);
    free(x3);
    free(x2);
    free(x1);
    return 0;
}

int goto_approach(void)
{
    int* x1 = malloc(1 * sizeof(int));
    if (!x1) {
        perror("oom");
        return 1;
    }

    int* x2 = malloc(1 * sizeof(int));
    if (!x2) {
        perror("oom");
        goto err_free_1;
    }

    int* x3 = malloc(1 * sizeof(int));
    if (!x3) {
        perror("oom");
        goto err_free_2;
    }

    int* x4 = malloc(1 * sizeof(int));
    if (!x4) {
        perror("oom");
        goto err_free_3;
    }

    free(x4);
    free(x3);
    free(x2);
    free(x1);
    return 0;

err_free_3:
    free(x3);

err_free_2:
    free(x2);

err_free_1:
    free(x1);
    return 1;
}

int defer_gotoless_approach(void)
{
    defer_scope_begin();

    int* x1 = malloc(1 * sizeof(int));
    if (!x1) {
        perror("oom");
        defer_scope_end();
        return 1;
    }
    defer_push_1(free, x1);

    int* x2 = malloc(1 * sizeof(int));
    if (!x2) {
        perror("oom");
        defer_scope_end();
        return 1;
    }
    defer_push_1(free, x2);

    int* x3 = malloc(1 * sizeof(int));
    if (!x3) {
        perror("oom");
        defer_scope_end();
        return 1;
    }
    defer_push_1(free, x3);

    int* x4 = malloc(1 * sizeof(int));
    if (!x4) {
        perror("oom");
        defer_scope_end();
        return 1;
    }
    defer_push_1(free, x4);

    defer_scope_end();
    return 0;
}
