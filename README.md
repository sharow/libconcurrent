libconcurrent
=============

tiny asymmetric-coroutine library.

## Description
+ asymmetric-coroutine
+ ~~generator~~ bidirectional communication with `yield_value`/`resume_value`
+ native context switch
+ ~~avoid another-lib dependency~~ C11
+ platform
 + Linux i686, X86_64, ARMv6(RaspberryPi), ARMv7(RaspberryPi2)
 + Windows i686 (mingw only)


## Code Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

#define STACK_SIZE (1024 * 2)


struct value { int a, b; };

noreturn void accumlator(struct concurrent_ctx *ctx)
{
    struct value *v = ctx_get_resume_value(ctx);
    for (;;) {
        int result = v->a + v->b;
        v = yield_value(ctx, &result); // send result / receive next value
    }
}

int main(void)
{
    struct concurrent_ctx *ctx;
    uint8_t stack[STACK_SIZE];
    uint8_t ctx_alloc[ctx_sizeof()];
    concurrent_init();
    ctx = (struct concurrent_ctx *)ctx_alloc;
    ctx_construct(ctx, stack, STACK_SIZE, accumlator, NULL);
    for (int i = 0; i < 10; i++) {
        int a = i;
        int b = 10 - i;
        int *result;
        result = resume_value(ctx, &(struct value){a, b}); // send value / receive result
        printf("%d + %d = %d\n", a, b, *result);
        
    }
    ctx_destruct(ctx);
    concurrent_fin();
    return EXIT_SUCCESS;
}

/*
$ gcc -o sample sample.c -lconcurrent.a
$ ./sample
0 + 10 = 10
1 + 9 = 10
2 + 8 = 10
3 + 7 = 10
4 + 6 = 10
5 + 5 = 10
6 + 4 = 10
7 + 3 = 10
8 + 2 = 10
9 + 1 = 10
*/
```

## Requirement for build
- nasm: http://www.nasm.us/


## Installation
```
$ git clone git@github.com:sharow/libconcurrent.git libconcurrent
$ cd libconcurrent
$ make
$ sudo make install

```

## Tests
```
$ make test

```

## License
[zlib](https://github.com/sharow/libconcurrent/blob/master/LICENSE)


## VS. 
+ Portable Coroutine Library (PCL): http://xmailserver.org/libpcl.html
+ libtask: https://code.google.com/p/libtask/
+ libconcurrency: http://code.google.com/p/libconcurrency/
+ libcoro: http://software.schmorp.de/pkg/libcoro.html
+ libcoroutine: https://github.com/stevedekorte/coroutine
+ coro: http://www.goron.de/~froese/coro/
+ libfiber: http://www.rkeene.org/projects/info/wiki/22
+ mill: https://github.com/sustrik/mill
+ fcontext: https://github.com/reginaldl/fcontext
+ libwire: https://github.com/baruch/libwire
