/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2016 sharow
*/

#ifndef __CONCURRENT_H__
#define __CONCURRENT_H__

#include <stddef.h>
#include <stdbool.h>


struct concurrent_ctx;
typedef void (*concurrent_proc)(struct concurrent_ctx *);

typedef enum {
    CONCURRENT_SUCCESS = 0,
    CONCURRENT_E_INVALID_ARGUMENT,
    CONCURRENT_E_STACKSIZE_TOO_SMALL
} concurrent_status;


void concurrent_init(void);
void concurrent_fin(void);
size_t concurrent_sizeof(void);

concurrent_status
concurrent_construct(struct concurrent_ctx *ctx,
                     void *stack_buffer,
                     size_t stack_size,
                     concurrent_proc proc,
                     void *user_ptr);

void concurrent_destruct(struct concurrent_ctx *ctx);

void *concurrent_resume_with_value(struct concurrent_ctx *ctx, void *value);
void *concurrent_resume(struct concurrent_ctx *ctx);
void *concurrent_get_resume_value(struct concurrent_ctx *ctx);
void *concurrent_yield_with_value(struct concurrent_ctx *ctx, void *value);
void *concurrent_yield(struct concurrent_ctx *ctx);
void *concurrent_get_yield_value(struct concurrent_ctx *ctx);
void concurrent_reset(struct concurrent_ctx *ctx);
void *concurrent_get_user_ptr(struct concurrent_ctx *ctx);
size_t concurrent_get_stack_used(struct concurrent_ctx *ctx);
void *concurrent_get_stack(struct concurrent_ctx *ctx);
bool concurrent_is_done(struct concurrent_ctx *ctx);


#endif
