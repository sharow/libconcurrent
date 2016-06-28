/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdnoreturn.h>
#include <concurrent/concurrent.h>
#include <concurrent/shortname.h>


struct strsplit {
    struct concurrent_ctx *concurrent;
    char *str;
    char c;
};

struct split_position {
    char *s;
    size_t len;
};

static void strsplit_coroutine(struct concurrent_ctx *ctx);

static struct strsplit *
strsplit_new(char *str, char c)
{
    struct strsplit *s;
    const size_t stack_size = 1024*4;
    s = malloc(sizeof(*s));
    s->concurrent = malloc(ctx_sizeof());
    ctx_construct(s->concurrent, malloc(stack_size), stack_size, strsplit_coroutine, s);
    s->str = str;
    s->c = c;
    return s;
}

static void
strsplit_del(struct strsplit *s)
{
    free(ctx_get_stack(s->concurrent));
    free(s->concurrent);
    free(s);
}

static void
strsplit_coroutine(struct concurrent_ctx *ctx)
{
    struct strsplit *s = ctx_get_user_ptr(ctx);
    char *p = s->str, *q = s->str;
    do {
        q = strchr(p, s->c);
        size_t len = q ? (size_t)(q - p) : strlen(p);
        yield_value(ctx, &(struct split_position){p, len});
        p = q + 1;
    } while (q);
}

struct split_position *
strsplit_next(struct strsplit *s)
{
    return resume(s->concurrent);
}


int main(void)
{
    concurrent_init();

    char *text = "monday|tuesday|wednesday|thursday|friday|saturday|sunday";
    struct strsplit *s = strsplit_new(text, '|');
    struct split_position *p;
    while ((p = strsplit_next(s)) != NULL) {
        printf("%.*s\n", (int)p->len, p->s);
    }
    strsplit_del(s);

    concurrent_fin();
    return EXIT_SUCCESS;
}

/* output

$ ./a.out 
monday
tuesday
wednesday
thursday
friday
saturday
sunday
*/
