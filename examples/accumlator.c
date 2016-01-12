/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

#define STACK_SIZE (1024 * 2)


struct value { int a, b; };

noreturn void accumlator(struct concurrent_ctx *ctx)
{
    struct value *v = ctx_get_resume_value(ctx);
    for (;;) {
        int result = v->a + v->b;
        v = yield_value(ctx, &result); // send result / receive next value
    }
}

int main(void)
{
    struct concurrent_ctx *ctx;
    uint8_t stack[STACK_SIZE];
    uint8_t ctx_alloc[ctx_sizeof()];
    concurrent_init();
    ctx = (struct concurrent_ctx *)ctx_alloc;
    ctx_construct(ctx, stack, STACK_SIZE, accumlator, NULL);
    for (int i = 0; i < 10; i++) {
        int a = i;
        int b = 10 - i;
        int *result;
        result = resume_value(ctx, &(struct value){a, b}); // send value / receive result
        printf("%d + %d = %d\n", a, b, *result);
        
    }
    ctx_destruct(ctx);
    concurrent_fin();
    return EXIT_SUCCESS;
}
