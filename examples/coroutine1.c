/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>


noreturn void coro(struct concurrent_ctx *ctx)
{
    int i = 0;
    for (;;) {
        printf("coro: %d\n", i++);
        yield(ctx);
    }
}

int main(void)
{
    struct concurrent_ctx *ctx;
    uint8_t *stack;
    const size_t stack_size = 1024*4;
    int i;

    concurrent_init();
    ctx = malloc(concurrent_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(ctx, stack, stack_size, coro, NULL);
    for (i = 0; i < 10; i++) {
        printf("main: %d\n", i);
        resume(ctx);
    }
    ctx_destruct(ctx);
    free(stack);
    free(ctx);
    concurrent_fin();
    return EXIT_SUCCESS;
}
