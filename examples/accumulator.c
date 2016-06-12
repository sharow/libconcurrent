/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

#define STACK_SIZE (1024 * 2)


noreturn void accumulator(struct concurrent_ctx *ctx)
{
    int *v = ctx_get_resume_value(ctx);
    int total = *v;
    for (;;) {
        v = yield_value(ctx, &total); // send total / receive next value
        total += *v;
    }
}

int main(void)
{
    struct concurrent_ctx *ctx;
    uint8_t stack[STACK_SIZE];
    uint8_t ctx_alloc[ctx_sizeof()];
    concurrent_init();
    ctx = (struct concurrent_ctx *)ctx_alloc;
    ctx_construct(ctx, stack, STACK_SIZE, accumulator, NULL);
    for (int i = 1; i <= 10; i++) {
        int *total = resume_value(ctx, &i); // send value / receive total
        printf("total = %d\n", *total);
        
    }
    ctx_destruct(ctx);
    concurrent_fin();
    return EXIT_SUCCESS;
}
