/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

#define NUM_CONTEXT 10000
#define NUM_LOOP 300

void coro(struct concurrent_ctx *ctx)
{
    int i;
    for (i = 0; i < NUM_LOOP; i++) {
        /* just count NUM_LOOP */
        yield(ctx);
    }
}

int main(void)
{
    static struct concurrent_ctx *contexts[NUM_CONTEXT];
    static uint8_t *stacks[NUM_CONTEXT];
    const size_t stack_size = 1024;
    int i;
    clock_t clock1, clock2;
    double diff_time;

    concurrent_init();

    /* construct */
    for (i = 0; i < NUM_CONTEXT; i++) {
        contexts[i] = malloc(ctx_sizeof());
        stacks[i] = malloc(sizeof(*stacks[i]) * stack_size);
        ctx_construct(contexts[i], stacks[i], stack_size, coro, NULL);
    }

    clock1 = clock();
    for (;;) {
        for (i = 0; i < NUM_CONTEXT; i++) {
            resume(contexts[i]);
        }
        if (ctx_is_done(contexts[0])) {
            break;
        }
    }
    clock2 = clock();

    diff_time = (clock2 - clock1) / (double)CLOCKS_PER_SEC;
    printf("%d context switch in %.1f ms\n", NUM_LOOP * NUM_CONTEXT, diff_time * 1000);
    printf("one context switch in %.0f ns\n", (diff_time * 1000000000) / (NUM_LOOP * NUM_CONTEXT));
    printf("%.0f resume/yield pair per second\n", NUM_LOOP * NUM_CONTEXT * (1.0 / diff_time));

    /* destruct */
    for (i = NUM_CONTEXT-1; i >= 0; i--) {
        ctx_destruct(contexts[i]);
        free(stacks[i]);
        free(contexts[i]);
    }
    concurrent_fin();
    return EXIT_SUCCESS;
}

/*
-- output: (Xeon E3 2.5Ghz)
3000000 context switch in 373.5 ms
one context switch in 125 ns
8031333 resume/yield pair per second


-- output: (RaspberryPi2 ARMv7 900MHz)
3000000 context switch in 2861.8 ms
one context switch in 954 ns
1048287 resume/yield pair per second

*/
