libconcurrent
=============

tiny asymmetric-coroutine library.

## Description
+ asymmetric-coroutine
+ generator
+ native context switch
+ avoid another-lib dependency
+ platform
 + Linux i686, X86_64
 + Windows i686 (mingw only)
 + ARMv6 (exclude VFP/NEON)


## Sample

```
#include <stdio.h>
#include <stdlib.h>
#include "concurrent/concurrent.h"
#include "concurrent/short_lower_case_api.h"

void coro(ConcurrentContext *aContext)
{
    int i = 0;
    for (;;) {
        printf("coro: %d\n", i++);
        yield(aContext);
    }
}

int main(void)
{
    ConcurrentContext *context;
    unsigned char stack[1024];
    int i;
    concurrent_init();
    context = malloc(ctx_sizeof());
    ctx_construct(context, stack, sizeof(stack), coro, NULL);
    for (i = 0; i < 10; i++) {
        ctx_resume(context);
    }
    ctx_destruct(context);
    free(context);
    concurrent_fin();
    return 0;
}

/*
$ gcc -o sample sample.c ./libconcurrent.a
$ ./sample
coro: 0
coro: 1
coro: 2
coro: 3
coro: 4
coro: 5
coro: 6
coro: 7
coro: 8
coro: 9
*/
```

## Requirement
- nasm: http://www.nasm.us/


## Install
```
$ git clone git@github.com:sharow/libconcurrent.git libconcurrent
$ cd libconcurrent
$ make depend
$ make
$ # and get libconcurrent.a

```


## Licence
[zlib](http://www.zlib.net/zlib_license.html)


## VS. 
+ Portable Coroutine Library (PCL): http://xmailserver.org/libpcl.html
+ libtask: https://code.google.com/p/libtask/
+ libconcurrency: http://code.google.com/p/libconcurrency/
+ libcoro: http://software.schmorp.de/pkg/libcoro.html
+ libcoroutine: https://github.com/stevedekorte/coroutine
+ coro: http://www.goron.de/~froese/coro/
+ libfiber: http://www.rkeene.org/projects/info/wiki/22

