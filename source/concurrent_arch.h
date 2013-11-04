/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2011 MIURA Shirow (sharow)
*/

#ifndef INCLUDED_CONCURRENT_ARCH_H
#define INCLUDED_CONCURRENT_ARCH_H

#include "concurrent/concurrent.h"


#ifndef CONCURRENT_ARCH_DECL
#define CONCURRENT_ARCH_DECL
#endif


extern CONCURRENT_ARCH_DECL int ConcurrentContext_Offsetof_Procedure;
extern CONCURRENT_ARCH_DECL int ConcurrentContext_Offsetof_StackPointer;
extern CONCURRENT_ARCH_DECL int ConcurrentContext_Offsetof_CallerReturnAddress;

CONCURRENT_ARCH_DECL void ConcurrentArch_SetupExecutionContext(ConcurrentContext *aContext);
CONCURRENT_ARCH_DECL void ConcurrentArch_TrampolineToProcedure(ConcurrentContext *aContext);
CONCURRENT_ARCH_DECL void ConcurrentArch_TrampolineToCaller(ConcurrentContext *aContext);

#endif
