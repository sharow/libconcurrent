/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

void coro(ConcurrentContext *aContext)
{
    float f;
    for (f = 0.0f; f < 2.0f; f += 0.2) {
        printf("coro: %f\n", f);
        yield(aContext);
    }
}

int main(void)
{
    ConcurrentContext *context;
    unsigned char *stack;
    float f;
    const int stack_size = 1024*4;

    concurrent_init();
    context = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(context, stack, stack_size, coro, NULL);
    for (f = 0.0f; f < 2.0f; f += 0.2f) {
        printf("main: %f\n", f);
        ctx_resume(context);
    }
    ctx_destruct(context);
    free(stack);
    free(context);
    concurrent_fin();
    return 0;
}
