/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

struct sort_ctx {
    int *data;
    int num_data;
};

struct concurrent_ctx *g_context;

void print_datas(struct sort_ctx *sort_ctx)
{
    int i;
    for (i = 0; i < sort_ctx->num_data; i++) {
        printf("%d ", sort_ctx->data[i]);
    }
}

int comp(const void *value0, const void *value1)
{
    int a = *(const int *)value0;
    int b = *(const int *)value1;

    yield(g_context);

    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    } else if (a == b) {
        return 0;
    }
    assert(!"not reached");
    return 0;
}

void coro(struct concurrent_ctx *ctx)
{
    struct sort_ctx *arg;
    arg = ctx_get_user_ptr(ctx);
    printf("qsort start\n");
    yield(g_context);
    qsort(arg->data, arg->num_data, sizeof(*arg->data), comp);
    yield(g_context);
    printf("qsort finish\n");
}

int main(void)
{
    uint8_t *stack;
    const int stack_size = 1024 * 4;
    int data[] = { 12, 13, 14, 15, 10, 9, 7, 1, 2, 11, 3, 4, 8, 6, 5, 0 };
    struct sort_ctx sort_arg;

    sort_arg.data = data;
    sort_arg.num_data =  sizeof(data) / sizeof(data[0]);

    concurrent_init();
    g_context = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(g_context, stack, stack_size, coro, (void *)&sort_arg);
    while (!ctx_is_done(g_context)) {
        size_t stack_size_used = ctx_get_stack_used(g_context);
        printf("stack used:%3zdbytes(%2.0f%%); data={ ",
               stack_size_used, ((float)stack_size_used / stack_size) * 100.0f);
        print_datas(&sort_arg);
        printf("}\n");
        resume(g_context);
    }

    ctx_destruct(g_context);
    free(stack);
    free(g_context);
    concurrent_fin();
    return EXIT_SUCCESS;
}

/* output:
stack used:0 bytes, data={ 9 7 1 2 3 4 8 6 5 0 }
qsort start
stack used:272 bytes, data={ 9 7 1 2 3 4 8 6 5 0 }
stack used:272 bytes, data={ 9 7 1 2 3 4 8 6 5 0 }
stack used:272 bytes, data={ 4 7 1 2 3 9 8 6 5 0 }
stack used:272 bytes, data={ 4 7 1 2 3 9 8 6 5 0 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:272 bytes, data={ 4 0 1 2 3 9 8 6 5 7 }
stack used:416 bytes, data={ 3 0 1 2 4 9 8 6 5 7 }
stack used:416 bytes, data={ 0 3 1 2 4 9 8 6 5 7 }
stack used:416 bytes, data={ 0 1 3 2 4 9 8 6 5 7 }
stack used:416 bytes, data={ 0 1 3 2 4 9 8 6 5 7 }
stack used:416 bytes, data={ 0 1 2 3 4 9 8 6 5 7 }
stack used:272 bytes, data={ 0 1 2 3 4 9 8 6 5 7 }
stack used:272 bytes, data={ 0 1 2 3 4 8 9 6 5 7 }
stack used:272 bytes, data={ 0 1 2 3 4 8 6 9 5 7 }
stack used:272 bytes, data={ 0 1 2 3 4 6 8 9 5 7 }
stack used:272 bytes, data={ 0 1 2 3 4 6 8 5 9 7 }
stack used:272 bytes, data={ 0 1 2 3 4 6 5 8 9 7 }
stack used:272 bytes, data={ 0 1 2 3 4 5 6 8 9 7 }
stack used:272 bytes, data={ 0 1 2 3 4 5 6 8 7 9 }
stack used:272 bytes, data={ 0 1 2 3 4 5 6 7 8 9 }
qsort finish
0 1 2 3 4 5 6 7 8 9 
 */
