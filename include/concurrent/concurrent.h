/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2013 MIURA Shirow (sharow)
*/

#ifndef INCLUDED_CONCURRENT_H
#define INCLUDED_CONCURRENT_H


#ifndef CONCURRENT_API
#define CONCURRENT_API
#endif

#ifndef CONCURRENT_CALLBACK
#define CONCURRENT_CALLBACK
#endif


typedef struct ConcurrentContext ConcurrentContext;
typedef void (*Concurrent_Procedure)(ConcurrentContext *context);

typedef enum {
    ConcurrentStatus_SUCCESS = 0,
    ConcurrentStatus_E_GENERIC,
    ConcurrentStatus_E_MODULE_NOT_INITIALIZED,
    ConcurrentStatus_E_INVALID_ARGUMENT,
    ConcurrentStatus_E_INVALID_OPERATION_AT_CURRENT_STATE,
    ConcurrentStatus_E_STACKSIZE_TOO_SMALL,
    ConcurrentStatus_E_GENERATOR_TYPE_MISMACH,
    ConcurrentStatus_E_FATAL
} ConcurrentStatus;

typedef enum {
    ConcurrentGeneratorValueType_VOID = 0,
    ConcurrentGeneratorValueType_INT,
    ConcurrentGeneratorValueType_UINT,
    ConcurrentGeneratorValueType_LONG,
    ConcurrentGeneratorValueType_ULONG,
    ConcurrentGeneratorValueType_LONGLONG,
    ConcurrentGeneratorValueType_ULONGLONG,
    ConcurrentGeneratorValueType_CONSTSTRING,
    ConcurrentGeneratorValueType_VOIDPTR
} ConcurrentGeneratorValueType;


/* -- API */

CONCURRENT_API ConcurrentStatus
Concurrent_Initialize(void);

CONCURRENT_API void
Concurrent_Finalize(void);

CONCURRENT_API const char *
Concurrent_GetStatusString(ConcurrentStatus aStatus);

CONCURRENT_API void
Concurrent_GetModuleInfo(int *outVersionMajor,
                         int *outVersionMinor,
                         int *outVersionRevision,
                         int *outHasLongLong,
                         int *outIsDebugBuild,
                         int *outIsStackGrowthDownward,
                         unsigned long *outMinimumStackSize,
                         const char **outLicenseName,
                         const char **outProjectURL,
                         const char **outBuildDateTime,
                         const char **outBuildInfo,
                         const char **outArchName);

/* -- context */
CONCURRENT_API int
ConcurrentContext_SizeOf(void);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Construct(ConcurrentContext *aContext,
                            unsigned char *aStackBuffer,
                            unsigned long aStackSize,
                            Concurrent_Procedure aProcedure,
                            void *aUserPtr);

CONCURRENT_API void
ConcurrentContext_Destruct(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Resume(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Reset(ConcurrentContext *aContext);

/* -- generator */

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Generate(ConcurrentContext *aContext, ConcurrentGeneratorValueType *outType);

CONCURRENT_API int
ConcurrentContext_GenerateInt(ConcurrentContext *aContext);

CONCURRENT_API unsigned int
ConcurrentContext_GenerateUInt(ConcurrentContext *aContext);

CONCURRENT_API long
ConcurrentContext_GenerateLong(ConcurrentContext *aContext);

CONCURRENT_API unsigned long
ConcurrentContext_GenerateULong(ConcurrentContext *aContext);

#ifdef CONCURRENT_HAS_LONGLONG
CONCURRENT_API long long
ConcurrentContext_GenerateLongLong(ConcurrentContext *aContext);

CONCURRENT_API unsigned long long
ConcurrentContext_GenerateULongLong(ConcurrentContext *aContext);
#endif

CONCURRENT_API void *
ConcurrentContext_GenerateVoidPtr(ConcurrentContext *aContext);

CONCURRENT_API const char *
ConcurrentContext_GenerateConstString(ConcurrentContext *aContext);


/* -- yield */

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Yield(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldVoid(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldInt(ConcurrentContext *aContext, int aValue);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldUInt(ConcurrentContext *aContext, unsigned int aValue);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldLong(ConcurrentContext *aContext, long aValue);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldULong(ConcurrentContext *aContext, unsigned long aValue);

#ifdef CONCURRENT_HAS_LONGLONG
CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldLongLong(ConcurrentContext *aContext, long long aValue);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldULongLong(ConcurrentContext *aContext, unsigned long long aValue);
#endif

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldVoidPtr(ConcurrentContext *aContext, void *aValue);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldConstString(ConcurrentContext *aContext, const char *aValue);


/* -- getter */

CONCURRENT_API ConcurrentGeneratorValueType
ConcurrentContext_GetGeneratedType(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedIntSafe(ConcurrentContext *aContext, int *outValue);

CONCURRENT_API int
ConcurrentContext_GetGeneratedInt(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedUIntSafe(ConcurrentContext *aContext, unsigned int *outValue);

CONCURRENT_API unsigned int
ConcurrentContext_GetGeneratedUInt(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedLongSafe(ConcurrentContext *aContext, long *outValue);

CONCURRENT_API long
ConcurrentContext_GetGeneratedLong(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedULongSafe(ConcurrentContext *aContext, unsigned long *outValue);

CONCURRENT_API unsigned long
ConcurrentContext_GetGeneratedULong(ConcurrentContext *aContext);

#ifdef CONCURRENT_HAS_LONGLONG
CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedLongLongSafe(ConcurrentContext *aContext, long long *outValue);

CONCURRENT_API long long
ConcurrentContext_GetGeneratedLongLong(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedULongLongSafe(ConcurrentContext *aContext, unsigned long long *outValue);

CONCURRENT_API unsigned long long
ConcurrentContext_GetGeneratedULongLong(ConcurrentContext *aContext);
#endif

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedVoidPtrSafe(ConcurrentContext *aContext, void **outValue);

CONCURRENT_API void *
ConcurrentContext_GetGeneratedVoidPtr(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedConstStringSafe(ConcurrentContext *aContext, const char **outValue);

CONCURRENT_API const char *
ConcurrentContext_GetGeneratedConstString(ConcurrentContext *aContext);


CONCURRENT_API void *
ConcurrentContext_GetUserPtr(ConcurrentContext *aContext);

CONCURRENT_API int
ConcurrentContext_IsDone(ConcurrentContext *aContext);

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetStackSizeUsed(ConcurrentContext *aContext, unsigned long *outStackSizeUsed);

#endif
