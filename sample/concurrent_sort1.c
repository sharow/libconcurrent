/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

typedef struct {
    int *mData;
    int mNumData;
} ConcurrentSortArg;

ConcurrentContext *g_context;

void print_datas(ConcurrentSortArg *aSortArg)
{
    int i;
    for (i = 0; i < aSortArg->mNumData; i++) {
        printf("%d ", aSortArg->mData[i]);
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

void coro(ConcurrentContext *aContext)
{
    ConcurrentSortArg *arg;
    arg = ctx_get_userptr(aContext);
    printf("qsort start\n");
    yield(g_context);
    qsort(arg->mData, arg->mNumData, sizeof(*arg->mData), comp);
    yield(g_context);
    printf("qsort finish\n");
}

int main(void)
{
    unsigned char *stack;
    const int stack_size = 1024 * 4;
    int data[] = { 12, 13, 14, 15, 10, 9, 7, 1, 2, 11, 3, 4, 8, 6, 5, 0 };
    ConcurrentSortArg sort_arg;

    sort_arg.mData = data;
    sort_arg.mNumData =  sizeof(data) / sizeof(data[0]);

    concurrent_init();
    g_context = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(g_context, stack, stack_size, coro, (void *)&sort_arg);
    while (!ctx_is_done(g_context)) {
        unsigned long stack_size_used;
        ctx_get_stack_used(g_context, &stack_size_used);
        printf("stack used:%3ub(%2.0f%%); data={ ",
               (unsigned int)stack_size_used,
               ((float)stack_size_used / stack_size) * 100.0f);
        print_datas(&sort_arg);
        printf("}\n");
        ctx_resume(g_context);
    }

    ctx_destruct(g_context);
    free(stack);
    free(g_context);
    concurrent_fin();
    return 0;
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
