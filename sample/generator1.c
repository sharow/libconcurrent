/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

void generator(ConcurrentContext *aContext)
{
    yield_voidptr(aContext, "hello");
    yield_voidptr(aContext, " ");
    yield_voidptr(aContext, "world");
    yield_voidptr(aContext, "!");
}

int main(void)
{
    ConcurrentContext *context;
    unsigned char *stack;
    const int stack_size = 1024 * 4;

    concurrent_init();
    context = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(context, stack, stack_size, generator, NULL);
    printf("generate: %s\n", (char *)gen_voidptr(context));
    printf("generate: %s\n", (char *)gen_voidptr(context));
    printf("generate: %s\n", (char *)gen_voidptr(context));
    printf("generate: %s\n", (char *)gen_voidptr(context));
    ctx_reset(context);
    printf("generate: %s\n", (char *)gen_voidptr(context));
    printf("generate: %s\n", (char *)gen_voidptr(context));
    printf("generate: %s\n", (char *)gen_voidptr(context));
    printf("generate: %s\n", (char *)gen_voidptr(context));
    ctx_destruct(context);

    free(stack);
    free(context);
    concurrent_fin();
    return 0;
}

