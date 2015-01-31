@ -*- Mode: text; tab-width: 4; indent-tabs-mode: nil; -*-
@
@  libconcurrent
@  Copyright (C) 2010-2015 MIURA Shirow (sharow)
@
@ for ARMv6
@ assemble with: GNU as
@

    .arch armv6
    .fpu vfp
    .align 2

    .text
    .global ConcurrentArch_SetupExecutionContext
    .global ConcurrentArch_TrampolineToProcedure
    .global ConcurrentArch_TrampolineToCaller

    .global ConcurrentContext_Offsetof_Procedure
    .global ConcurrentContext_Offsetof_StackPointer
    .global ConcurrentContext_Offsetof_CallerReturnAddress


@
@ void ConcurrentArch_SetupExecutionContext(ConcurrentContext *aContext)
@
ConcurrentArch_SetupExecutionContext:
    @ r0 = aContext
    ldr r1, global_var
    ldr r2, [r1, #+4]   @ ConcurrentContext_Offsetof_StackPointer
    ldr r3, [r0, r2]    @ mStackPointer
    mov r2, sp          @ save sp to r2
    mov sp, r3          @ change stack

    @ push initial register values
    stmfd sp!, {r0}     @ for EABI stack alignment(64bit)
    stmfd sp!, {r0}     @ aContext for ConcurrentArch_ReturnAtProcedure

    ldr r3, [r1, #+0]   @ ConcurrentContext_Offsetof_Procedure
    ldr r3, [r0, r3]    @ r3 = mProcedure
    stmfd sp!, {r3}     @ lr

    eor r3, r3
    stmfd sp!, {r3}     @ r11
    stmfd sp!, {r3}     @ r10
    stmfd sp!, {r3}     @ r9
    stmfd sp!, {r3}     @ r8
    stmfd sp!, {r3}     @ r7
    stmfd sp!, {r3}     @ r6
    stmfd sp!, {r3}     @ r5
    stmfd sp!, {r3}     @ r4

    vpush {d0-d15}

    ldr r3, [r1, #+4]   @ ConcurrentContext_Offsetof_StackPointer
    str sp, [r0, r3]    @ save to mStackPointer

    mov sp, r2          @ restore sp
    mov pc, lr


@
@ void ConcurrentArch_TrampolineToProcedure(ConcurrentContext *aContext)
@
ConcurrentArch_TrampolineToProcedure:
    @ r0 = aContext

    stmfd sp!, {r4-r11, lr}
    vpush {d0-d15}

    ldr r1, global_var

    @ save Return Address
    ldr r2, [r1, #+8]   @ ConcurrentContext_Offsetof_CallerReturnAddress
    str lr, [r0, r2]    @ lr = mCallerReturnAddress

    @ exchange stack
    ldr r2, [r1, #+4]   @ ConcurrentContext_Offsetof_StackPointer
    ldr r3, [r0, r2]    @ r3 = mStackPointer
    str sp, [r0, r2]    @ mStackPointer = sp
    mov sp, r3          @ sp = r3

    adr lr, ConcurrentArch_ReturnAtProcedure

    vpop {d0-d15}
    ldmfd sp!, {r4-r11, pc}

@
@ void ConcurrentArch_TrampolineToCaller(ConcurrentContext *aContext)
@
ConcurrentArch_TrampolineToCaller:
    @ r0 = aContext

    stmfd sp!, {r4-r11, lr}
    vpush {d0-d15}

    ldr r1, global_var

    @ exchange stack
    ldr r2, [r1, #+4]   @ ConcurrentContext_Offsetof_StackPointer
    ldr r3, [r0, r2]    @ r3 = mStackPointer
    str sp, [r0, r2]    @ mStackPointer = sp
    mov sp, r3          @ sp = r3

    vpop {d0-d15}
    ldmfd sp!, {r4-r11, pc}


ConcurrentArch_ReturnAtProcedure:
    ldmfd sp!, {r0}     @ aContext

    ldr r1, global_var

    @ restore stack
    ldr r2, [r1, #+4]   @ ConcurrentContext_Offsetof_StackPointer
    ldr r3, [r0, r2]    @ r3 = mStackPointer
    mov sp, r3          @ sp = r3

    vpop {d0-d15}
    ldmfd sp!, {r4-r11, pc}


@@@@

    .align 4
global_var: .long _global_var


@@@@
    .data
    .align 2
_global_var:
ConcurrentContext_Offsetof_Procedure:           .long 0
ConcurrentContext_Offsetof_StackPointer:        .long 0
ConcurrentContext_Offsetof_CallerReturnAddress: .long 0
