/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

#define STACK_SIZE (1024 * 2)


/* coroutine in coroutine recursive */
void coro(struct concurrent_ctx *ctx)
{
    struct concurrent_ctx *ctx2;
    uint8_t *stack;
    int deep;

    deep = (int)ctx_get_user_ptr(ctx);
    if (deep == 0) {
        return;
    }

    printf("%*scoro(deep=%d) start\n", deep, " ", deep);

    ctx2 = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * STACK_SIZE);
    ctx_construct(ctx2, stack, STACK_SIZE, coro, (void *)(deep - 1));
    while (!ctx_is_done(ctx2)) {
        printf("%*scoro(deep=%d) first\n", deep, " ", deep);
        resume(ctx2);
        yield(ctx);
    }
    ctx_reset(ctx2);
    while (!ctx_is_done(ctx2)) {
        printf("%*scoro(deep=%d) second\n", deep, " ", deep);
        resume(ctx2);
        yield(ctx);
    }
    ctx_destruct(ctx2);
    free(stack);
    free(ctx2);
    printf("%*scoro(deep=%d) finish\n", deep, " ", deep);
}

int main(void)
{
    struct concurrent_ctx *ctx;
    uint8_t *stack;

    concurrent_init();
    ctx = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * STACK_SIZE);
    ctx_construct(ctx, stack, STACK_SIZE, coro, /* deep = */(void *)3);
    while (!ctx_is_done(ctx)) {
        printf("main()\n");
        resume(ctx);
    }
    ctx_destruct(ctx);
    free(stack);
    free(ctx);
    concurrent_fin();
    printf("main() finish\n");
    return EXIT_SUCCESS;
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
