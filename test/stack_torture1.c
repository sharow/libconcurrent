/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>


void test_generator(struct concurrent_ctx *ctx)
{
    yield_value(ctx, "pong");
}

int main(void)
{
    int i;

    srand(time(NULL));
    concurrent_init();
    for (i = 0; i < 1000; i++) {
        size_t stack_size = 1024 + 16 + (rand() % 16);
        size_t torture_offset = rand() % 16;
        struct concurrent_ctx *ctx;
        uint8_t *stack;
        concurrent_status status;

        ctx = malloc(ctx_sizeof());
        stack = malloc(sizeof(*stack) * stack_size);
        status = ctx_construct(ctx,
                               stack + torture_offset,
                               stack_size - torture_offset,
                               test_generator, NULL);

        assert(status == CONCURRENT_SUCCESS);
        assert(strcmp(resume(ctx), "pong") == 0);

        ctx_destruct(ctx);
        free(stack);
        free(ctx);
    }
    concurrent_fin();
    return 0;
}
