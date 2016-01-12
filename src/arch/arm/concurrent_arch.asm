@ -*- Mode: text; tab-width: 4; indent-tabs-mode: nil; -*-
@
@  libconcurrent
@  Copyright (C) 2010-2016 sharow
@
@ for ARMv6
@ assemble with: GNU as
@

    .arch armv6
    .fpu vfp
    .align 2

    .text
    .global concurrent_arch_setup_execution_context
    .global concurrent_arch_trampoline_to_procedure
    .global concurrent_arch_trampoline_to_caller

    .global concurrent_offsetof_procedure
    .global concurrent_offsetof_stack_ptr
    .global concurrent_offsetof_caller_return_addr


@
@ void concurrent_arch_setup_execution_context(struct concurrent_ctx *ctx)
@
concurrent_arch_setup_execution_context:
    @ r0 = ctx
    ldr r1, global_var
    ldr r2, [r1, #+4]   @ concurrent_offsetof_stack_ptr
    ldr r3, [r0, r2]    @ stack_ptr
    mov r2, sp          @ save sp to r2
    mov sp, r3          @ change stack

    @ push initial register values
    stmfd sp!, {r0}     @ for EABI stack alignment(64bit)
    stmfd sp!, {r0}     @ ctx for concurrent_arch_return_at_procedure()

    ldr r3, [r1, #+0]   @ concurrent_offsetof_procedure
    ldr r3, [r0, r3]    @ r3 = proc
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

    ldr r3, [r1, #+4]   @ concurrent_offsetof_stack_ptr
    str sp, [r0, r3]    @ save to stack_ptr

    mov sp, r2          @ restore sp
    mov pc, lr


@
@ void concurrent_arch_trampoline_to_procedure(struct concurrent_ctx *ctx)
@
concurrent_arch_trampoline_to_procedure:
    @ r0 = ctx

    stmfd sp!, {r4-r11, lr}
    vpush {d0-d15}

    ldr r1, global_var

    @ exchange stack
    ldr r2, [r1, #+4]   @ concurrent_offsetof_stack_ptr
    ldr r3, [r0, r2]    @ r3 = stack_ptr
    str sp, [r0, r2]    @ stack_ptr = sp
    mov sp, r3          @ sp = r3

    adr lr, concurrent_arch_return_at_procedure

    vpop {d0-d15}
    ldmfd sp!, {r4-r11, pc}

@
@ void concurrent_arch_trampoline_to_caller(struct concurrent_ctx *ctx)
@
concurrent_arch_trampoline_to_caller:
    @ r0 = ctx

    stmfd sp!, {r4-r11, lr}
    vpush {d0-d15}

    ldr r1, global_var

    @ exchange stack
    ldr r2, [r1, #+4]   @ concurrent_offsetof_stack_ptr
    ldr r3, [r0, r2]    @ r3 = stack_ptr
    str sp, [r0, r2]    @ stack_ptr = sp
    mov sp, r3          @ sp = r3

    vpop {d0-d15}
    ldmfd sp!, {r4-r11, pc}


concurrent_arch_return_at_procedure:
    ldmfd sp!, {r0}     @ ctx

    ldr r1, global_var

    @ restore stack
    ldr r2, [r1, #+4]   @ concurrent_offsetof_stack_ptr
    ldr r3, [r0, r2]    @ r3 = stack_ptr
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
concurrent_offsetof_procedure:           .long 0
concurrent_offsetof_stack_ptr:           .long 0
concurrent_offsetof_caller_return_addr:  .long 0
