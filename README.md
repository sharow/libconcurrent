libconcurrent
=============
[![zlib License](http://img.shields.io/badge/license-zlib-orange.svg?style=flat-square)](https://github.com/sharow/libconcurrent/blob/master/LICENSE)


tiny asymmetric-coroutine library.

## Description
+ asymmetric-coroutine
+ bidirectional communication by `yield_value`/`resume_value`
+ native context switch
+ C11

## Supported Platforms
|         | x86_64              | i686                     | ARM(v6/v7)               | note             |
|---------|---------------------|--------------------------|--------------------------|------------------|
| Linux   | :heavy_check_mark:  | :heavy_check_mark:       | :heavy_check_mark:       |                  |
| FreeBSD | :heavy_check_mark:  | :heavy_check_mark:       | :question:               |Thanks [@t6](https://github.com/t6)|
| OSX     | :heavy_check_mark:  | :heavy_multiplication_x: | :heavy_multiplication_x: |Thanks [@kpamnany](https://github.com/kpamnany) |

## Code Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>

#define STACK_SIZE (1024 * 2)


noreturn void accumulator(struct concurrent_ctx *ctx)
{
    int *v = ctx_get_resume_value(ctx);
    int total = *v;
    for (;;) {
        v = yield_value(ctx, &total); // send total / receive next value
        total += *v;
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
    for (int i = 1; i <= 10; i++) {
        int *total = resume_value(ctx, &i); // send value / receive total
        printf("total = %d\n", *total);
        
    }
    ctx_destruct(ctx);
    concurrent_fin();
    return EXIT_SUCCESS;
}

/*
$ gcc -o sample sample.c -lconcurrent.a
$ ./sample
total = 1
total = 3
total = 6
total = 10
total = 15
total = 21
total = 28
total = 36
total = 45
total = 55
*/
```

## Requirements for build
- [nasm](http://www.nasm.us/)


## Installation
```
$ git clone https://github.com/sharow/libconcurrent.git libconcurrent
$ cd libconcurrent
$ make
$ sudo make install

```

#### for FreeBSD
Available in ports collection as [devel/libconcurrent](http://portsmon.freebsd.org/portoverview.py?category=devel&portname=libconcurrent)


#### for OSX
- install latest [nasm](http://www.nasm.us/) from [brew](http://brew.sh)

```
$ brew install nasm
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
