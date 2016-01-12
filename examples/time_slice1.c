/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#define _GNU_SOURCE
#include <features.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <time.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

#define MS 1
#define TIME_SLICE (30 / MS)

const struct timespec g_sleep_ts = {
    .tv_sec = 0,
    .tv_nsec = 1000 * 1000 * 10
};

noreturn void task0(struct concurrent_ctx *ctx)
{
    clock_t clk;
    clk = clock();
    for (;;) {
        printf("A");
        fflush(stdout);
        if (((clock() - clk) * 1000.0 / CLOCKS_PER_SEC) > TIME_SLICE) {
            printf("\ntask0: yield\n");
            yield(ctx);
            clk = clock();
        }
        nanosleep(&g_sleep_ts, NULL);
    }
}

noreturn void task1(struct concurrent_ctx *ctx)
{
    clock_t clk;
    clk = clock();
    for (;;) {
        printf("B");
        fflush(stdout);
        if (((clock() - clk) * 1000.0 / CLOCKS_PER_SEC) > TIME_SLICE) {
            printf("\ntask1: yield\n");
            yield(ctx);
            clk = clock();
        }
        nanosleep(&g_sleep_ts, NULL);
    }
}

int main(void)
{
    struct concurrent_ctx *ctx0;
    struct concurrent_ctx *ctx1;
    uint8_t *stack0;
    uint8_t *stack1;
    const size_t stack_size = 1024 * 2;

    concurrent_init();

    ctx0 = malloc(ctx_sizeof());
    stack0 = malloc(sizeof(*stack0) * stack_size);
    ctx_construct(ctx0, stack0, stack_size, task0, NULL);

    ctx1 = malloc(ctx_sizeof());
    stack1 = malloc(sizeof(*stack1) * stack_size);
    ctx_construct(ctx1, stack1, stack_size, task1, NULL);

    {
        int i;
        for (i = 0; i < 10; i++) {
            printf("main: resume task0\n");
            resume(ctx0);
            printf("main: resume task1\n");
            resume(ctx1);
        }
    }

    printf("finish\n");

    ctx_destruct(ctx1);
    free(stack1);
    free(ctx1);
    ctx_destruct(ctx0);
    free(stack0);
    free(ctx0);

    concurrent_fin();
    return EXIT_SUCCESS;
}
