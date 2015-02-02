;; -*- Mode: text; tab-width: 4; indent-tabs-mode: nil; -*-
;;
;;  libconcurrent
;;  Copyright (C) 2010-2011 MIURA Shirow (sharow)
;;
;; for 386 or later.
;; assemble with:
;;  NASM: http://www.nasm.us/
;;  or YASM: http://www.tortall.net/projects/yasm/
;;

section .text

global ConcurrentArch_SetupExecutionContext
global ConcurrentArch_TrampolineToProcedure
global ConcurrentArch_TrampolineToCaller

global ConcurrentContext_Offsetof_Procedure
global ConcurrentContext_Offsetof_StackPointer
global ConcurrentContext_Offsetof_CallerReturnAddress


;;
;; void ConcurrentArch_SetupExecutionContext(ConcurrentContext *aContext)
;;
ConcurrentArch_SetupExecutionContext:
    mov ecx, dword [esp + 4]    ; aContext

    mov eax, dword [ConcurrentContext_Offsetof_StackPointer]
    xchg esp, dword [ecx + eax] ; exchange stack

    push ecx                                 ; aContext for "return" or out of scope
    push ecx                                 ; aContext for yield
    push ConcurrentArch_TrampolineToCaller  ; return to ConcurrentArch_TrampolineToCaller(aContext)

    mov eax, dword [ConcurrentContext_Offsetof_Procedure]
    push dword [ecx + eax] ; entry point

    ; initial register value
    xor eax, eax
    push eax  ; ebx
    push eax  ; esi
    push eax  ; edi
    push eax  ; ebp

    mov eax, dword [ConcurrentContext_Offsetof_StackPointer]
    xchg esp, dword [ecx + eax] ; restore stack

    ret


;;
;; void ConcurrentArch_TrampolineToProcedure(ConcurrentContext *aContext)
;;
ConcurrentArch_TrampolineToProcedure:
    mov ecx, dword [esp + 4]    ; aContext

    push ebx
    push esi
    push edi
    push ebp

    ; save return address
    mov edx, dword [esp + 16]      ; return address of this function
    mov eax, dword [ConcurrentContext_Offsetof_CallerReturnAddress]
    mov dword [ecx + eax], edx

    ; exchange stack
    mov eax, dword [ConcurrentContext_Offsetof_StackPointer]
    xchg esp, dword [ecx + eax]
    nop

    ; now changed context
    pop ebp
    pop edi
    pop esi
    pop ebx

    ret


;;
;; void ConcurrentArch_TrampolineToCaller(ConcurrentContext *aContext)
;;
ConcurrentArch_TrampolineToCaller:
    ;; 2 ways: function-call or return from procedure("return" in procedure or out of scope).

    mov ecx, dword [esp + 4]   ; aContext

    push ebx
    push esi
    push edi
    push ebp

    ; exchange stack
    mov eax, dword [ConcurrentContext_Offsetof_StackPointer]
    xchg dword [ecx + eax], esp
    nop

    ; get return address
    mov eax, dword [ConcurrentContext_Offsetof_CallerReturnAddress]
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

ConcurrentContext_Offsetof_Procedure           dd 0
ConcurrentContext_Offsetof_StackPointer        dd 0
ConcurrentContext_Offsetof_CallerReturnAddress dd 0
