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

#define TIME_SLICE_SEC (0.1)


struct task_info {
    const char *out;
};

const struct timespec g_sleep_ts = {
    .tv_sec = 0,
    .tv_nsec = 1000 * 1000 * 5
}; // 5 ms

noreturn void task(struct concurrent_ctx *ctx)
{
    struct task_info *info = ctx_get_user_ptr(ctx);
    struct timespec t0 = {0}, t1 = {0};
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (;;) {
        printf("%s", info->out);
        fflush(stdout);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double s0 = t0.tv_sec + t0.tv_nsec / 1.0e9;
        double s1 = t1.tv_sec + t1.tv_nsec / 1.0e9;
        if ((s1 - s0) > TIME_SLICE_SEC) {
            printf("\ntask %s: yield\n", info->out);
            yield(ctx);
            clock_gettime(CLOCK_MONOTONIC, &t0);
        }
        if (nanosleep(&g_sleep_ts, NULL)) {
            abort();
        }
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
    ctx_construct(ctx0, stack0, stack_size, task, &(struct task_info){"A"});

    ctx1 = malloc(ctx_sizeof());
    stack1 = malloc(sizeof(*stack1) * stack_size);
    ctx_construct(ctx1, stack1, stack_size, task, &(struct task_info){"B"});

    {
        int i;
        for (i = 0; i < 10; i++) {
            printf("main:\n");
            resume(ctx0);
            printf("main:\n");
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
