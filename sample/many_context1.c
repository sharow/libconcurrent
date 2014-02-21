/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

#define NUM_CONTEXT 100000
#define NUM_LOOP 30

void coro(ConcurrentContext *aContext)
{
    int i;
    for (i = 0; i < NUM_LOOP; i++) {
        /* just count NUM_LOOP */
        yield(aContext);
    }
}

int main(void)
{
    static ConcurrentContext *contexts[NUM_CONTEXT];
    static unsigned char *stacks[NUM_CONTEXT];
    const int stack_size = 1024;
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
            ctx_resume(contexts[i]);
        }
        if (ctx_is_done(contexts[0])) {
            break;
        }
    }
    clock2 = clock();

    diff_time = (clock2 - clock1) / (double)CLOCKS_PER_SEC;
    printf("%d context switch in %.1f ms\n", NUM_LOOP * NUM_CONTEXT, diff_time * 1000);
    printf("one context switch in %.0f ns\n", (diff_time * 1000000000) / (NUM_LOOP * NUM_CONTEXT));
    printf("context switch about %.0f times in 1 sec\n", NUM_LOOP * NUM_CONTEXT * (1.0 / diff_time));

    /* destruct */
    for (i = NUM_CONTEXT-1; i >= 0; i--) {
        ctx_destruct(contexts[i]);
        free(stacks[i]);
        free(contexts[i]);
    }
    concurrent_fin();
    return 0;
}

/* output: (on CoreSolo 1.06GHz)
3000000 context switch in 1062.0 ms
one context switch in 354 ns
context switch about 2824859 times in 1 sec
*/
