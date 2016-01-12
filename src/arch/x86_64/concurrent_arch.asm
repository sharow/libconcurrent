;; -*- Mode: text; tab-width: 4; indent-tabs-mode: nil; -*-
;;
;;  libconcurrent
;;  Copyright (C) 2010-2016 sharow
;;
;; for x86_64, gcc-abi
;; assemble with:
;;  NASM: http://www.nasm.us/
;;

section .text

global concurrent_arch_setup_execution_context
global concurrent_arch_trampoline_to_procedure
global concurrent_arch_trampoline_to_caller

global concurrent_offsetof_procedure
global concurrent_offsetof_stack_ptr
global concurrent_offsetof_caller_return_addr

%macro save_context 0
    push rbx
    push rbp
    push rdi
    push rsi
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro restore_context 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop rsi
    pop rdi
    pop rbp
    pop rbx
%endmacro

;;
;; void concurrent_arch_setup_execution_context(struct concurrent_ctx *ctx)
;;
concurrent_arch_setup_execution_context:
    mov rcx, rdi

    ; exchange stack
    mov rax, qword [concurrent_offsetof_stack_ptr]
    xchg rsp, qword [rcx + rax]

    push rcx                                 ; ctx for "return" or out of scope
    push rcx                                 ; ctx for yield
    push concurrent_arch_return_at_procedure

    mov rax, qword [concurrent_offsetof_procedure]
    push qword [rcx + rax] ; entry point

    ; initial register value
    xor rax, rax
    push rax  ; rbx
    push rax  ; rbp
    push rcx  ; rdi, 1st argument
    push rax  ; rsi
    push rax  ; r12
    push rax  ; r13
    push rax  ; r14
    push rax  ; r15

    ; restore stack
    mov rax, qword [concurrent_offsetof_stack_ptr]
    xchg rsp, qword [rcx + rax]

    ret


;;
;; void concurrent_arch_trampoline_to_procedure(struct concurrent_ctx *ctx)
;;
concurrent_arch_trampoline_to_procedure:
    mov rcx, rdi

    save_context

    ; save return address
    mov rdx, qword [rsp + 8 * 8]      ; return address of this function
    mov rax, qword [concurrent_offsetof_caller_return_addr]
    mov qword [rcx + rax], rdx

    ; exchange stack
    mov rax, qword [concurrent_offsetof_stack_ptr]
    xchg rsp, qword [rcx + rax]
    nop

    restore_context

    ret


;;
;; void concurrent_arch_trampoline_to_caller(struct concurrent_ctx *ctx)
;;
concurrent_arch_trampoline_to_caller:
    mov rcx, rdi

    save_context

    ; exchange stack
    mov rax, qword [concurrent_offsetof_stack_ptr]
    xchg qword [rcx + rax], rsp
    nop

    ; get return address
    mov rax, qword [concurrent_offsetof_caller_return_addr]
    mov rax, qword [rcx + rax]

    restore_context

    add rsp, 8  ; remove return address
    nop
    nop
    jmp rax     ; jump to return address


concurrent_arch_return_at_procedure:
    pop rcx   ; Retuen address
    pop rcx   ; ctx

    save_context

    ; exchange stack
    mov rax, qword [concurrent_offsetof_stack_ptr]
    xchg qword [rcx + rax], rsp
    nop

    ; get return address
    mov rax, qword [concurrent_offsetof_caller_return_addr]
    mov rax, qword [rcx + rax]

    restore_context

    add rsp, 8  ; remove return address
    nop
    nop
    jmp rax     ; jump to return address


section .data
concurrent_offsetof_procedure           dq 0
concurrent_offsetof_stack_ptr           dq 0
concurrent_offsetof_caller_return_addr  dq 0

