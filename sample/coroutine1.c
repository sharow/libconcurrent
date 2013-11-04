/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

void coro(ConcurrentContext *aContext)
{
    int i = 0;
    for (;;) {
        printf("coro: %d\n", i++);
        yield(aContext);
    }
}

int main(void)
{
    ConcurrentContext *context;
    unsigned char *stack;
    int i;
    const int stack_size = 1024*4;

    concurrent_init();
    context = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(context, stack, stack_size, coro, NULL);
    for (i = 0; i < 10; i++) {
        printf("main: %d\n", i);
        ctx_resume(context);
    }
    ctx_destruct(context);
    free(stack);
    free(context);
    concurrent_fin();
    return 0;
}
