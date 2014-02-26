/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "concurrent/concurrent.h"
#include "concurrent/short_lower_case_api.h"

void test_generator(ConcurrentContext *aContext)
{
    yield_str(aContext, "pong");
}

int main(void)
{
    int i;

    srand(time(NULL));
    concurrent_init();
    for (i = 0; i < 1000; i++) {
        int stack_size = 1024 + 16 + (rand() % 16);
        int torture_offset = rand() % 16;
        ConcurrentContext *context;
        unsigned char *stack;
        ConcurrentStatus status;

        context = malloc(ctx_sizeof());
        stack = malloc(sizeof(*stack) * stack_size);
        status = ctx_construct(context,
                               stack + torture_offset,
                               stack_size - torture_offset,
                               test_generator, NULL);

        assert(status == ConcurrentStatus_SUCCESS);
        assert(strcmp(gen_str(context), "pong") == 0);

        ctx_destruct(context);
        free(stack);
        free(context);
    }
    concurrent_fin();
    return 0;
}
