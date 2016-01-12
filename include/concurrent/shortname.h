/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2016 sharow
*/

#ifndef __SHORTNAME_H__
#define __SHORTNAME_H__

#include <concurrent/concurrent.h>

#define ctx_sizeof concurrent_sizeof
#define ctx_construct concurrent_construct
#define ctx_destruct concurrent_destruct
#define resume_value concurrent_resume_with_value
#define resume concurrent_resume
#define ctx_get_resume_value concurrent_get_resume_value
#define yield_value concurrent_yield_with_value
#define yield concurrent_yield
#define ctx_get_yiled_value concurrent_get_yield_value
#define ctx_reset concurrent_reset
#define ctx_get_user_ptr concurrent_get_user_ptr
#define ctx_get_stack_used concurrent_get_stack_used
#define ctx_is_done concurrent_is_done


#endif
