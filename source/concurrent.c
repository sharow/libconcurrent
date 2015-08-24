/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2015 MIURA Shirow (sharow)
*/

#ifdef CONCURRENT_HAS_BUILD_CONFIG
#include "concurrent_build_config.h"
#endif

#include "concurrent/concurrent.h"

#include "concurrent_arch.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef CONCURRENT_MIN_STACK_SIZE
#define CONCURRENT_MIN_STACK_SIZE (1024)
#endif

#ifndef CONCURRENT_CPU_REGISTER_TYPE
# if defined(__SIZEOF_POINTER__) && defined(__SIZEOF_LONG_LONG__)
#  if __SIZEOF_POINTER__ == __SIZEOF_LONG_LONG__
#   define CONCURRENT_CPU_REGISTER_TYPE unsigned long long
#  else
#   define CONCURRENT_CPU_REGISTER_TYPE unsigned long
#  endif
# elif defined(CONCURRENT_HAS_LONGLONG)
#  define CONCURRENT_CPU_REGISTER_TYPE unsigned long long
# else
#  define CONCURRENT_CPU_REGISTER_TYPE unsigned long
# endif
#endif


typedef CONCURRENT_CPU_REGISTER_TYPE Concurrent_CPURegister_t;
typedef CONCURRENT_CPU_REGISTER_TYPE Concurrent_StackSize_t;


typedef enum {
    ConcurrentContextState_SETUP,
    ConcurrentContextState_EXECUTE,
    ConcurrentContextState_YIELD,
    ConcurrentContextState_DONE
} ConcurrentContextState;

typedef struct {
    ConcurrentGeneratorValueType mType;
    union {
        int mInt;
        unsigned int mUInt;
        long mLong;
        unsigned long mULong;
        const char *mConstString;
        void *mVoidPtr;
#ifdef CONCURRENT_HAS_LONGLONG
        long long mLongLong;
        unsigned long long mULongLong;
#endif
    } uValue;
} ConcurrentGeneratorValue;

struct ConcurrentContext {
    Concurrent_Procedure mProcedure;
    unsigned char *mStackBuffer; /* must be align */
    Concurrent_StackSize_t mStackSize;
    void *mUserPtr;

    Concurrent_CPURegister_t mStackBase;
    Concurrent_CPURegister_t mStackPointer;
    void *mCallerReturnAddress;
    ConcurrentContextState mState;
    ConcurrentGeneratorValue mGeneratorValue;
};


/* -- static prototype */
static void
ConcurrentContext_SetupExecutionContext(ConcurrentContext *aContext);


/* -- module */

#define OFFSETOF(obj, member) ((Concurrent_CPURegister_t) ((char *)&((obj *)(0))->member - (char *)0))

CONCURRENT_API ConcurrentStatus
Concurrent_Initialize(void)
{
    ConcurrentContext_Offsetof_Procedure = OFFSETOF(ConcurrentContext, mProcedure);
    ConcurrentContext_Offsetof_StackPointer = OFFSETOF(ConcurrentContext, mStackPointer);
    ConcurrentContext_Offsetof_CallerReturnAddress = OFFSETOF(ConcurrentContext, mCallerReturnAddress);
    return ConcurrentStatus_SUCCESS;
}

#undef OFFSETOF

CONCURRENT_API void
Concurrent_Finalize(void)
{
}

CONCURRENT_API const char *
Concurrent_GetStatusString(ConcurrentStatus aStatus)
{
    static const char * const status_strs[] = {
        "SUCCESS",
        "ERROR_GENERIC",
        "ERROR_MODULE_NOT_INITIALIZED",
        "ERROR_INVALID_ARGUMENT",
        "ERROR_INVALID_OPERATION_AT_CURRENT_STATE",
        "ERROR_STACKSIZE_TOO_SMALL",
        "ERROR_GENERATOR_TYPE_MISMACH",
        "ERROR_FATAL"
    };
    if (aStatus < ConcurrentStatus_SUCCESS || aStatus > ConcurrentStatus_E_FATAL) {
        return NULL;
    }
    return status_strs[aStatus];
}

#pragma GCC diagnostic ignored "-Wreturn-type"
CONCURRENT_API int
Concurrent_GetStackDirection(int iter)
{
	// 1 = grows downward
	// 0 = grows upward
    static unsigned long baseptr;
    int depth;
    if ( iter == 0 ) baseptr = (unsigned long) &depth;
    if ( iter < 10 ) {
        Concurrent_GetStackDirection(iter + 1);
    } else {
        return ( (unsigned long)&depth + (baseptr - (unsigned long)&depth) == baseptr ? 1 : 0  );
    }
}

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
                         const char **outArchName)
{
    if (outVersionMajor) *outVersionMajor = CONCURRENT_VERSION_MAJOR;
    if (outVersionMinor) *outVersionMinor = CONCURRENT_VERSION_MINOR;
    if (outVersionRevision) *outVersionRevision = CONCURRENT_VERSION_REVISION;

    if (outHasLongLong) {
#ifdef CONCURRENT_HAS_LONGLONG
        *outHasLongLong = 1;
#else
        *outHasLongLong = 0;
#endif
    }

    if (outIsDebugBuild) {
#ifdef CONCURRENT_DEBUG
        *outIsDebugBuild = 1;
#else
        *outIsDebugBuild = 0;
#endif
    }
    if (outIsStackGrowthDownward) *outIsStackGrowthDownward = Concurrent_GetStackDirection(0);
    if (outMinimumStackSize) *outMinimumStackSize = (unsigned long)CONCURRENT_MIN_STACK_SIZE;

    if (outLicenseName) *outLicenseName = "zlib License";
    if (outProjectURL) *outProjectURL = "https://github.com/sharow/libconcurrent";
    if (outBuildDateTime) {
#ifdef __DATE__
# ifdef __TIME__
        *outBuildDateTime = __DATE__ " "  __TIME__;
# else
        *outBuildDateTime = __DATE__;
# endif
#else
        *outBuildDateTime = "unknown";
#endif
    }
    if (outBuildInfo) {
#ifdef __VERSION__
        *outBuildInfo = "GCC " __VERSION__;
#else
        *outBuildInfo = "";
#endif
    }
    if (outArchName) {
#ifdef CONCURRENT_ARCH_NAME
        *outArchName = CONCURRENT_ARCH_NAME;
#else
        *outArchName = "unknown";
#endif
    }
}

/* -- ConcurrentContext */

CONCURRENT_API int
ConcurrentContext_SizeOf(void)
{
    return sizeof(ConcurrentContext);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Construct(ConcurrentContext *aContext,
                            unsigned char *aStackBuffer,
                            unsigned long aStackSize,
                            Concurrent_Procedure aProcedure,
                            void *aUserPtr)
{
    if (aContext && aStackBuffer && aProcedure) {
    } else {
        return ConcurrentStatus_E_INVALID_ARGUMENT;
    }
    if (aStackSize < CONCURRENT_MIN_STACK_SIZE) {
        return ConcurrentStatus_E_STACKSIZE_TOO_SMALL;
    }
    aContext->mProcedure = aProcedure;
    aContext->mStackBuffer = (unsigned char *)((Concurrent_CPURegister_t)(aStackBuffer + 15) & ~15);
    aContext->mStackSize = (aStackSize + aStackBuffer - aContext->mStackBuffer) & ~15;
    aContext->mUserPtr = aUserPtr;

    /* TODO: stack direction */
    aContext->mStackBase = (Concurrent_CPURegister_t)(aContext->mStackBuffer + aContext->mStackSize);
    aContext->mStackPointer = 0;
    aContext->mCallerReturnAddress = NULL;
    aContext->mState = ConcurrentContextState_SETUP;
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_VOID;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API void
ConcurrentContext_Destruct(ConcurrentContext *aContext)
{
    /* for safe */
    aContext->mProcedure = NULL;
    aContext->mStackBuffer = NULL;
    aContext->mStackSize = 0;
    aContext->mUserPtr = NULL;
    aContext->mStackBase = 0;
    aContext->mStackPointer = 0;
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_VOID;
}


CONCURRENT_API ConcurrentStatus
ConcurrentContext_Resume(ConcurrentContext *aContext)
{
    switch (aContext->mState) {
    case ConcurrentContextState_SETUP:
        ConcurrentContext_SetupExecutionContext(aContext);
        /* fall through */

    case ConcurrentContextState_YIELD:
        aContext->mState = ConcurrentContextState_EXECUTE;
        aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_VOID;
        ConcurrentArch_TrampolineToProcedure(aContext);
        if (aContext->mState == ConcurrentContextState_EXECUTE) {
            /* not yeild */
            aContext->mState = ConcurrentContextState_DONE;
        }
        return ConcurrentStatus_SUCCESS;

    case ConcurrentContextState_EXECUTE:
        /* Resume() at callee(==procedure): this is invalid! */
        return ConcurrentStatus_E_INVALID_OPERATION_AT_CURRENT_STATE;

    case ConcurrentContextState_DONE:
        return ConcurrentStatus_E_INVALID_OPERATION_AT_CURRENT_STATE;

    default:
        return ConcurrentStatus_E_FATAL;
    }

    /* not reached */
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Reset(ConcurrentContext *aContext)
{
    if (aContext->mState == ConcurrentContextState_EXECUTE) {
        return ConcurrentStatus_E_INVALID_OPERATION_AT_CURRENT_STATE;
    }
    aContext->mState = ConcurrentContextState_SETUP;
    return ConcurrentStatus_SUCCESS;
}


/* -- generator */

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Generate(ConcurrentContext *aContext, ConcurrentGeneratorValueType *outType)
{
    ConcurrentStatus status;
    status = ConcurrentContext_Resume(aContext);
    if (status != ConcurrentStatus_SUCCESS) {
        return status;
    }
    *outType = aContext->mGeneratorValue.mType;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API int
ConcurrentContext_GenerateInt(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mInt = 0;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mInt;
}

CONCURRENT_API unsigned int
ConcurrentContext_GenerateUInt(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mUInt = 0;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mUInt;
}

CONCURRENT_API long
ConcurrentContext_GenerateLong(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mLong = 0;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mLong;
}

CONCURRENT_API unsigned long
ConcurrentContext_GenerateULong(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mULong = 0;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mULong;
}





#ifdef CONCURRENT_HAS_LONGLONG
CONCURRENT_API long long
ConcurrentContext_GenerateLongLong(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mLongLong = 0;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mLongLong;
}

CONCURRENT_API unsigned long long
ConcurrentContext_GenerateULongLong(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mULongLong = 0;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mULongLong;
}


#endif

CONCURRENT_API void *
ConcurrentContext_GenerateVoidPtr(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mVoidPtr = NULL;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mVoidPtr;
}


CONCURRENT_API const char *
ConcurrentContext_GenerateConstString(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.uValue.mConstString = NULL;
    ConcurrentContext_Resume(aContext);
    return aContext->mGeneratorValue.uValue.mConstString;
}


/* -- yield */

CONCURRENT_API ConcurrentStatus
ConcurrentContext_Yield(ConcurrentContext *aContext)
{
    if (aContext->mState != ConcurrentContextState_EXECUTE) {
        return ConcurrentStatus_E_INVALID_OPERATION_AT_CURRENT_STATE;
    }
    aContext->mState = ConcurrentContextState_YIELD;
    ConcurrentArch_TrampolineToCaller(aContext);
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldVoid(ConcurrentContext *aContext)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_VOID;
    return ConcurrentContext_Yield(aContext);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldInt(ConcurrentContext *aContext, int aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_INT;
    aContext->mGeneratorValue.uValue.mInt = aValue;
    return ConcurrentContext_Yield(aContext);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldUInt(ConcurrentContext *aContext, unsigned int aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_UINT;
    aContext->mGeneratorValue.uValue.mUInt = aValue;
    return ConcurrentContext_Yield(aContext);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldLong(ConcurrentContext *aContext, long aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_LONG;
    aContext->mGeneratorValue.uValue.mLong = aValue;
    return ConcurrentContext_Yield(aContext);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldULong(ConcurrentContext *aContext, unsigned long aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_ULONG;
    aContext->mGeneratorValue.uValue.mULong = aValue;
    return ConcurrentContext_Yield(aContext);
}

#ifdef CONCURRENT_HAS_LONGLONG
CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldLongLong(ConcurrentContext *aContext, long long aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_LONGLONG;
    aContext->mGeneratorValue.uValue.mLongLong = aValue;
    return ConcurrentContext_Yield(aContext);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldULongLong(ConcurrentContext *aContext, unsigned long long aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_ULONGLONG;
    aContext->mGeneratorValue.uValue.mULongLong = aValue;
    return ConcurrentContext_Yield(aContext);
}
#endif

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldVoidPtr(ConcurrentContext *aContext, void *aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_VOIDPTR;
    aContext->mGeneratorValue.uValue.mVoidPtr = aValue;
    return ConcurrentContext_Yield(aContext);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_YieldConstString(ConcurrentContext *aContext, const char *aValue)
{
    aContext->mGeneratorValue.mType = ConcurrentGeneratorValueType_CONSTSTRING;
    aContext->mGeneratorValue.uValue.mConstString = aValue;
    return ConcurrentContext_Yield(aContext);
}


/* -- get generated value */

CONCURRENT_API ConcurrentGeneratorValueType
ConcurrentContext_GetGeneratedType(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.mType;
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedIntSafe(ConcurrentContext *aContext, int *outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_INT) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mInt;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API int
ConcurrentContext_GetGeneratedInt(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mInt;
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedUIntSafe(ConcurrentContext *aContext, unsigned int *outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_UINT) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mUInt;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API unsigned int
ConcurrentContext_GetGeneratedUInt(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mUInt;
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedLongSafe(ConcurrentContext *aContext, long *outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_LONG) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mLong;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API long
ConcurrentContext_GetGeneratedLong(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mLong;
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedULongSafe(ConcurrentContext *aContext, unsigned long *outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_ULONG) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mULong;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API unsigned long
ConcurrentContext_GetGeneratedULong(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mULong;
}

#ifdef CONCURRENT_HAS_LONGLONG
CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedLongLongSafe(ConcurrentContext *aContext, long long *outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_LONGLONG) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mLongLong;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API long long
ConcurrentContext_GetGeneratedLongLong(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mLongLong;
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedULongLongSafe(ConcurrentContext *aContext, unsigned long long *outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_ULONGLONG) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mULongLong;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API unsigned long long
ConcurrentContext_GetGeneratedULongLong(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mULongLong;
}
#endif

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedVoidPtrSafe(ConcurrentContext *aContext, void **outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_VOIDPTR) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mVoidPtr;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API void *
ConcurrentContext_GetGeneratedVoidPtr(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mVoidPtr;
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetGeneratedConstStringSafe(ConcurrentContext *aContext, const char **outValue)
{
    if (aContext->mGeneratorValue.mType != ConcurrentGeneratorValueType_CONSTSTRING) {
        return ConcurrentStatus_E_GENERATOR_TYPE_MISMACH;
    }
    *outValue = aContext->mGeneratorValue.uValue.mConstString;
    return ConcurrentStatus_SUCCESS;
}

CONCURRENT_API const char *
ConcurrentContext_GetGeneratedConstString(ConcurrentContext *aContext)
{
    return aContext->mGeneratorValue.uValue.mConstString;
}


CONCURRENT_API void *
ConcurrentContext_GetUserPtr(ConcurrentContext *aContext)
{
    return aContext->mUserPtr;
}

CONCURRENT_API int
ConcurrentContext_IsDone(ConcurrentContext *aContext)
{
    return (aContext->mState == ConcurrentContextState_DONE);
}

CONCURRENT_API ConcurrentStatus
ConcurrentContext_GetStackSizeUsed(ConcurrentContext *aContext, unsigned long *outStackSizeUsed)
{
    /* TODO: stack direction */

    if (!outStackSizeUsed) {
        return ConcurrentStatus_E_INVALID_ARGUMENT;
    }
    if (aContext->mState == ConcurrentContextState_SETUP) {
        *outStackSizeUsed = 0;
        return ConcurrentStatus_E_INVALID_OPERATION_AT_CURRENT_STATE;
    }
    if (aContext->mState == ConcurrentContextState_EXECUTE) {
        /* invald */
        *outStackSizeUsed = 0;
        return ConcurrentStatus_E_INVALID_OPERATION_AT_CURRENT_STATE;
    }

    *outStackSizeUsed = (unsigned long)(aContext->mStackBase - aContext->mStackPointer);
    return ConcurrentStatus_SUCCESS;
}


static void
ConcurrentContext_SetupExecutionContext(ConcurrentContext *aContext)
{
    /* TODO: stack direction */

    aContext->mStackPointer = (Concurrent_CPURegister_t)aContext->mStackBase;

    ConcurrentArch_SetupExecutionContext(aContext);
}
