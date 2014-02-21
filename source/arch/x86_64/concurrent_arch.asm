;; -*- Mode: text; tab-width: 4; indent-tabs-mode: nil; -*-
;;
;;  libconcurrent
;;  Copyright (C) 2010-2013 MIURA Shirow (sharow)
;;
;; for x86_64, gcc-abi
;; assemble with:
;;  NASM: http://www.nasm.us/
;;

section .text

global ConcurrentArch_SetupExecutionContext
global ConcurrentArch_TrampolineToProcedure
global ConcurrentArch_TrampolineToCaller

global ConcurrentContext_Offsetof_Procedure
global ConcurrentContext_Offsetof_StackPointer
global ConcurrentContext_Offsetof_CallerReturnAddress

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
;; void ConcurrentArch_SetupExecutionContext(ConcurrentContext *aContext)
;;
ConcurrentArch_SetupExecutionContext:
    mov rcx, rdi

    ; exchange stack
    mov rax, qword [ConcurrentContext_Offsetof_StackPointer]
    xchg rsp, qword [rcx + rax]

    push rcx                                 ; aContext for "return" or out of scope
    push rcx                                 ; aContext for yield
    push ConcurrentArch_ReturnAtProcedure

    mov rax, qword [ConcurrentContext_Offsetof_Procedure]
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
    mov rax, qword [ConcurrentContext_Offsetof_StackPointer]
    xchg rsp, qword [rcx + rax]

    ret


;;
;; void ConcurrentArch_TrampolineToProcedure(ConcurrentContext *aContext)
;;
ConcurrentArch_TrampolineToProcedure:
    mov rcx, rdi

    save_context

    ; save return address
    mov rdx, qword [rsp + 8 * 8]      ; return address of this function
    mov rax, qword [ConcurrentContext_Offsetof_CallerReturnAddress]
    mov qword [rcx + rax], rdx

    ; exchange stack
    mov rax, qword [ConcurrentContext_Offsetof_StackPointer]
    xchg rsp, qword [rcx + rax]
    nop

    restore_context

    ret


;;
;; void ConcurrentArch_TrampolineToCaller(ConcurrentContext *aContext)
;;
ConcurrentArch_TrampolineToCaller:
    mov rcx, rdi

    save_context

    ; exchange stack
    mov rax, qword [ConcurrentContext_Offsetof_StackPointer]
    xchg qword [rcx + rax], rsp
    nop

    ; get return address
    mov rax, qword [ConcurrentContext_Offsetof_CallerReturnAddress]
    mov rax, qword [rcx + rax]

    restore_context

    add rsp, 8  ; remove return address
    nop
    nop
    jmp rax     ; jump to return address


ConcurrentArch_ReturnAtProcedure:
    pop rcx   ; Retuen address
    pop rcx   ; aContext

    save_context

    ; exchange stack
    mov rax, qword [ConcurrentContext_Offsetof_StackPointer]
    xchg qword [rcx + rax], rsp
    nop

    ; get return address
    mov rax, qword [ConcurrentContext_Offsetof_CallerReturnAddress]
    mov rax, qword [rcx + rax]

    restore_context

    add rsp, 8  ; remove return address
    nop
    nop
    jmp rax     ; jump to return address


section .data

ConcurrentContext_Offsetof_Procedure           dq 0
ConcurrentContext_Offsetof_StackPointer        dq 0
ConcurrentContext_Offsetof_CallerReturnAddress dq 0
