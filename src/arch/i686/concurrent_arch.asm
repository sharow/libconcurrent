;; -*- Mode: text; tab-width: 4; indent-tabs-mode: nil; -*-
;;
;;  libconcurrent
;;  Copyright (C) 2010-2016 sharow
;;
;; for 386 or later.
;; assemble with:
;;  NASM: http://www.nasm.us/
;;  or YASM: http://www.tortall.net/projects/yasm/
;;

section .text

global concurrent_arch_setup_execution_context
global concurrent_arch_trampoline_to_procedure
global concurrent_arch_trampoline_to_caller

global concurrent_offsetof_procedure
global concurrent_offsetof_stack_ptr
global concurrent_offsetof_caller_return_addr


;;
;; void concurrent_arch_setup_execution_context(struct concurrent_ctx *ctx)
;;
concurrent_arch_setup_execution_context:
    mov ecx, dword [esp + 4]    ; ctx

    mov eax, dword [concurrent_offsetof_stack_ptr]
    xchg esp, dword [ecx + eax] ; exchange stack

    push ecx                                 ; ctx for "return" or out of scope
    push ecx                                 ; ctx for yield
    push concurrent_arch_trampoline_to_caller  ; return to concurrent_arch_trampoline_to_caller(ctx)

    mov eax, dword [concurrent_offsetof_procedure]
    push dword [ecx + eax] ; entry point

    ; initial register value
    xor eax, eax
    push eax  ; ebx
    push eax  ; esi
    push eax  ; edi
    push eax  ; ebp

    mov eax, dword [concurrent_offsetof_stack_ptr]
    xchg esp, dword [ecx + eax] ; restore stack

    ret


;;
;; void concurrent_arch_trampoline_to_procedure(struct concurrent_ctx *ctx)
;;
concurrent_arch_trampoline_to_procedure:
    mov ecx, dword [esp + 4]    ; ctx

    push ebx
    push esi
    push edi
    push ebp

    ; save return address
    mov edx, dword [esp + 16]      ; return address of this function
    mov eax, dword [concurrent_offsetof_caller_return_addr]
    mov dword [ecx + eax], edx

    ; exchange stack
    mov eax, dword [concurrent_offsetof_stack_ptr]
    xchg esp, dword [ecx + eax]
    nop

    ; now changed context
    pop ebp
    pop edi
    pop esi
    pop ebx

    ret


;;
;; void concurrent_arch_trampoline_to_caller(struct concurrent_ctx *ctx)
;;
concurrent_arch_trampoline_to_caller:
    ;; 2 ways: function-call or return from procedure("return" in procedure or out of scope).

    mov ecx, dword [esp + 4]   ; ctx

    push ebx
    push esi
    push edi
    push ebp

    ; exchange stack
    mov eax, dword [concurrent_offsetof_stack_ptr]
    xchg dword [ecx + eax], esp
    nop

    ; get return address
    mov eax, dword [concurrent_offsetof_caller_return_addr]
    mov eax, dword [ecx + eax]

    pop ebp
    pop edi
    pop esi
    pop ebx

    add esp, 4  ; remove return address
    nop
    nop
    jmp eax     ; jump to return address


section .data
concurrent_offsetof_procedure          dd 0
concurrent_offsetof_stack_ptr          dd 0
concurrent_offsetof_caller_return_addr dd 0
