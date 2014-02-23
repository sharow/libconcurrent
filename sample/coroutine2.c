/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

#define STACK_SIZE (1024 * 2)


/* coroutine in coroutine recursive */
void coro(ConcurrentContext *aContext)
{
    ConcurrentContext *context;
    unsigned char *stack;
    int deep;

    deep = (int)ctx_get_userptr(aContext);
    if (deep == 0) {
        return;
    }

    printf("%*scoro(deep=%d) start\n", deep, " ", deep);

    context = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * STACK_SIZE);
    ctx_construct(context, stack, STACK_SIZE, coro, (void *)(deep - 1));
    while (!ctx_is_done(context)) {
        printf("%*scoro(deep=%d) first\n", deep, " ", deep);
        ctx_resume(context);
        yield(aContext);
    }
    ctx_reset(context);
    while (!ctx_is_done(context)) {
        printf("%*scoro(deep=%d) second\n", deep, " ", deep);
        ctx_resume(context);
        yield(aContext);
    }
    ctx_destruct(context);
    free(stack);
    free(context);
    printf("%*scoro(deep=%d) finish\n", deep, " ", deep);
}

int main(void)
{
    ConcurrentContext *context;
    unsigned char *stack;

    concurrent_init();
    context = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * STACK_SIZE);
    ctx_construct(context, stack, STACK_SIZE, coro, /* deep = */(void *)3);
    while (!ctx_is_done(context)) {
        printf("main()\n");
        ctx_resume(context);
    }
    ctx_destruct(context);
    free(stack);
    free(context);
    concurrent_fin();
    printf("main() finish\n");
    return 0;
}

/* output:
main()
   coro(deep=3) start
   coro(deep=3) first
  coro(deep=2) start
  coro(deep=2) first
 coro(deep=1) start
 coro(deep=1) first
main()
   coro(deep=3) first
  coro(deep=2) first
 coro(deep=1) second
main()
   coro(deep=3) first
  coro(deep=2) first
 coro(deep=1) finish
main()
   coro(deep=3) first
  coro(deep=2) second
 coro(deep=1) start
 coro(deep=1) first
main()
   coro(deep=3) first
  coro(deep=2) second
 coro(deep=1) second
main()
   coro(deep=3) first
  coro(deep=2) second
 coro(deep=1) finish
main()
   coro(deep=3) first
  coro(deep=2) finish
main()
   coro(deep=3) second
  coro(deep=2) start
  coro(deep=2) first
 coro(deep=1) start
 coro(deep=1) first
main()
   coro(deep=3) second
  coro(deep=2) first
 coro(deep=1) second
main()
   coro(deep=3) second
  coro(deep=2) first
 coro(deep=1) finish
main()
   coro(deep=3) second
  coro(deep=2) second
 coro(deep=1) start
 coro(deep=1) first
main()
   coro(deep=3) second
  coro(deep=2) second
 coro(deep=1) second
main()
   coro(deep=3) second
  coro(deep=2) second
 coro(deep=1) finish
main()
   coro(deep=3) second
  coro(deep=2) finish
main()
   coro(deep=3) finish
main() finish
*/
