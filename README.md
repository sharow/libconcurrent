libconcurrent
=============
[![zlib License](http://img.shields.io/badge/license-zlib-orange.svg?style=flat-square)](https://github.com/sharow/libconcurrent/blob/master/LICENSE)


tiny asymmetric-coroutine library.

## Description
+ asymmetric-coroutine
+ ~~generator~~ bidirectional communication with `yield_value`/`resume_value`
+ native context switch
+ ~~avoid another-lib dependency~~ C11

## Supported Platforms
|         | x86_64                   | i686               | ARM(v6/v7)               | note             |
|---------|--------------------------|--------------------|--------------------------|------------------|
| Linux   | :heavy_check_mark:       | :heavy_check_mark: | :heavy_check_mark:       |                  |
| FreeBSD | :heavy_check_mark:       | :heavy_check_mark: | :question:               |Thanks [@t6](https://github.com/t6)|
| Windows | :heavy_multiplication_x: | :heavy_check_mark: | :heavy_multiplication_x: | use MSys2/Cygwin |

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

noreturn void accumulator(struct concurrent_ctx *ctx)
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
    ctx_construct(ctx, stack, STACK_SIZE, accumulator, NULL);
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

## Requirements for build
- nasm: http://www.nasm.us/


## Installation
```
$ git clone https://github.com/sharow/libconcurrent.git libconcurrent
$ cd libconcurrent
$ make
$ sudo make install

```

## Tests
```
$ make test

```

## Benchmark

[examples/many_context1.c](https://github.com/sharow/libconcurrent/blob/master/examples/many_context1.c):

```
-- output: (Xeon E3 2.5Ghz)
3000000 context switch in 373.5 ms
one context switch in 125 ns
8031333 resume/yield pair per second


-- output: (RaspberryPi2 ARMv7 900MHz)
3000000 context switch in 2861.8 ms
one context switch in 954 ns
1048287 resume/yield pair per second
```


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
+ coroutine(A asymmetric coroutine library for C): https://github.com/cloudwu/coroutine
+ coroutine(a asymmetric coroutine (lua like) with fixed-size stack): https://github.com/xphh/coroutine
+ coroutine(coroutine library with pthread-like interface in pure C): https://github.com/Marcus366/coroutine
+ coroutines(A lightweight coroutine library written in C and assembler): https://github.com/xya/coroutines
+ micro: https://github.com/mikewei/micoro
