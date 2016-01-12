/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2016 sharow
*/

#ifndef __CONCURRENT_ARCH_H__
#define __CONCURRENT_ARCH_H__

#include <stddef.h>
#include "concurrent/concurrent.h"


extern size_t concurrent_offsetof_procedure;
extern size_t concurrent_offsetof_stack_ptr;
extern size_t concurrent_offsetof_caller_return_addr;

void concurrent_arch_setup_execution_context(struct concurrent_ctx *ctx);
void concurrent_arch_trampoline_to_procedure(struct concurrent_ctx *ctx);
void concurrent_arch_trampoline_to_caller(struct concurrent_ctx *ctx);


#endif
