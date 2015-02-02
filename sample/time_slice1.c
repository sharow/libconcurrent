/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#define _GNU_SOURCE
#include <features.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

#define MS 1
#define TIME_SLICE (20 / MS)

const struct timespec g_sleep_ts = {
    .tv_sec = 0,
    .tv_nsec = 1000 * 1000 * 10
};

void task0(ConcurrentContext *aContext)
{
    clock_t clk;
    clk = clock();
    for (;;) {
        printf("A");
        fflush(stdout);
        if (((clock() - clk) * 1000.0 / CLOCKS_PER_SEC) > TIME_SLICE) {
            printf("\ntask0: yield\n");
            yield(aContext);
            clk = clock();
        }
        nanosleep(&g_sleep_ts, NULL);
    }
}

void task1(ConcurrentContext *aContext)
{
    clock_t clk;
    clk = clock();
    for (;;) {
        printf("B");
        fflush(stdout);
        if (((clock() - clk) * 1000.0 / CLOCKS_PER_SEC) > TIME_SLICE) {
            printf("\ntask1: yield\n");
            yield(aContext);
            clk = clock();
        }
        nanosleep(&g_sleep_ts, NULL);
    }
}

int main(void)
{
    ConcurrentContext *context0;
    ConcurrentContext *context1;
    unsigned char *stack0;
    unsigned char *stack1;
    const int stack_size = 1024 * 2;

    concurrent_init();

    context0 = malloc(ctx_sizeof());
    stack0 = malloc(sizeof(*stack0) * stack_size);
    ctx_construct(context0, stack0, stack_size, task0, NULL);

    context1 = malloc(ctx_sizeof());
    stack1 = malloc(sizeof(*stack1) * stack_size);
    ctx_construct(context1, stack1, stack_size, task1, NULL);

    {
        int i;
        for (i = 0; i < 10; i++) {
            printf("main: resume task0\n");
            ctx_resume(context0);
            printf("main: resume task1\n");
            ctx_resume(context1);
        }
    }

    printf("finish\n");

    ctx_destruct(context1);
    free(stack1);
    free(context1);
    ctx_destruct(context0);
    free(stack0);
    free(context0);

    concurrent_fin();
    return 0;
}
