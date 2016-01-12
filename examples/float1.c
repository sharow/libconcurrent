/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

void coro(struct concurrent_ctx *ctx)
{
    float f;
    for (f = 0.0f; f < 2.0f; f += 0.2) {
        printf("coro: %f\n", f);
        yield(ctx);
    }
}

int main(void)
{
    struct concurrent_ctx *ctx;
    uint8_t *stack;
    const size_t stack_size = 1024*4;
    float f;

    concurrent_init();
    ctx = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(ctx, stack, stack_size, coro, NULL);
    for (f = 0.0f; f < 2.0f; f += 0.2f) {
        printf("main: %f\n", f);
        resume(ctx);
    }
    ctx_destruct(ctx);
    free(stack);
    free(ctx);
    concurrent_fin();
    return EXIT_SUCCESS;
}
