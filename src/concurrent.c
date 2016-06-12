/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2016 sharow
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <assert.h>

#include "concurrent/concurrent.h"
#include "concurrent_arch.h"

#ifndef CONCURRENT_MIN_STACK_SIZE
# define CONCURRENT_MIN_STACK_SIZE (1024)
#endif


typedef enum {
    CONCURRENT_STATE_SETUP,
    CONCURRENT_STATE_EXECUTE,
    CONCURRENT_STATE_YIELD,
    CONCURRENT_STATE_DONE
} concurrent_state;

struct concurrent_ctx {
    concurrent_proc proc;
    void *user_ptr;
    void *stack_orig;
    size_t stack_size;
    uintptr_t stack_base;
    uintptr_t stack_ptr;
    concurrent_state state;
    void *caller_return_addr;
    void *yield_value;
    void *resume_value;
};


noreturn static void
panic(void)
{
    abort();
}

static void
concurrent_setup_execution_context(struct concurrent_ctx *ctx)
{
    ctx->stack_ptr = ctx->stack_base;
    concurrent_arch_setup_execution_context(ctx);
}


void
concurrent_init(void)
{
    concurrent_offsetof_procedure = offsetof(struct concurrent_ctx, proc);
    concurrent_offsetof_stack_ptr = offsetof(struct concurrent_ctx, stack_ptr);
    concurrent_offsetof_caller_return_addr = offsetof(struct concurrent_ctx, caller_return_addr);
}

void
concurrent_fin(void)
{
}


size_t
concurrent_sizeof(void)
{
    return sizeof(struct concurrent_ctx);
}

concurrent_status
concurrent_construct(struct concurrent_ctx *ctx,
                     void *stack_buffer,
                     size_t stack_size,
                     concurrent_proc proc,
                     void *user_ptr)
{
    const uintptr_t align = 16 - 1;
    uintptr_t aligned_stack_buffer;
    if (ctx && stack_buffer && proc) {
    } else {
        return CONCURRENT_E_INVALID_ARGUMENT;
    }
    if (stack_size < CONCURRENT_MIN_STACK_SIZE) {
        return CONCURRENT_E_STACKSIZE_TOO_SMALL;
    }
    aligned_stack_buffer = ((uintptr_t)stack_buffer + align) & ~align;
    assert((uintptr_t)aligned_stack_buffer >= (uintptr_t)stack_buffer);
    ctx->proc = proc;
    ctx->user_ptr = user_ptr;
    ctx->stack_orig = stack_buffer;
    ctx->stack_size = (stack_size - (size_t)(aligned_stack_buffer - (uintptr_t)stack_buffer)) & ~(size_t)align;
    ctx->stack_base = aligned_stack_buffer + ctx->stack_size;
    ctx->stack_ptr = 0;
    ctx->state = CONCURRENT_STATE_SETUP;
    ctx->caller_return_addr = NULL;
    ctx->yield_value = NULL;
    ctx->resume_value = NULL;
    return CONCURRENT_SUCCESS;
}

void
concurrent_destruct(struct concurrent_ctx *ctx)
{
    ctx->proc = NULL;
    ctx->user_ptr = NULL;
    ctx->stack_orig = NULL;
    ctx->stack_size = 0;
    ctx->stack_base = 0;
    ctx->stack_ptr = 0;
    ctx->state = CONCURRENT_STATE_DONE;
    ctx->caller_return_addr = NULL;
    ctx->yield_value = NULL;
    ctx->resume_value = NULL;
}

void *
concurrent_resume_with_value(struct concurrent_ctx *ctx, void *value)
{
    switch (ctx->state) {
    case CONCURRENT_STATE_SETUP:
        concurrent_setup_execution_context(ctx);
        // fall through

    case CONCURRENT_STATE_YIELD:
        ctx->state = CONCURRENT_STATE_EXECUTE;
        ctx->yield_value = NULL;
        ctx->resume_value = value;
        concurrent_arch_trampoline_to_procedure(ctx);
        if (ctx->state == CONCURRENT_STATE_EXECUTE) {
            // not yeild
            ctx->state = CONCURRENT_STATE_DONE;
            return NULL;
        } else {
            return ctx->yield_value;
        }
    case CONCURRENT_STATE_EXECUTE:
    case CONCURRENT_STATE_DONE:
    default:
        panic();
    }
    return NULL;
}

void *
concurrent_resume(struct concurrent_ctx *ctx)
{
    return concurrent_resume_with_value(ctx, NULL);
}

void *
concurrent_get_resume_value(struct concurrent_ctx *ctx)
{
    return ctx->resume_value;
}

void *
concurrent_yield_with_value(struct concurrent_ctx *ctx, void *value)
{
    if (ctx->state != CONCURRENT_STATE_EXECUTE) {
        panic();
    }
    ctx->yield_value = value;
    ctx->state = CONCURRENT_STATE_YIELD;
    concurrent_arch_trampoline_to_caller(ctx);
    return ctx->resume_value;
}

void *
concurrent_yield(struct concurrent_ctx *ctx)
{
    return concurrent_yield_with_value(ctx, NULL);
}

void *
concurrent_get_yield_value(struct concurrent_ctx *ctx)
{
    return ctx->yield_value;
}

void
concurrent_reset(struct concurrent_ctx *ctx)
{
    if (ctx->state == CONCURRENT_STATE_EXECUTE) {
        panic();
    }
    ctx->state = CONCURRENT_STATE_SETUP;
}

void *
concurrent_get_user_ptr(struct concurrent_ctx *ctx)
{
    return ctx->user_ptr;
}

bool
concurrent_is_done(struct concurrent_ctx *ctx)
{
    return (ctx->state == CONCURRENT_STATE_DONE) ? true : false;
}

size_t
concurrent_get_stack_used(struct concurrent_ctx *ctx)
{
    if (ctx->state == CONCURRENT_STATE_SETUP) {
        return 0;
    }
    if (ctx->state == CONCURRENT_STATE_EXECUTE) {
        panic();
    }
    return (size_t)(ctx->stack_base - ctx->stack_ptr);
}

void *
concurrent_get_stack(struct concurrent_ctx *ctx)
{
    return ctx->stack_orig;
}
