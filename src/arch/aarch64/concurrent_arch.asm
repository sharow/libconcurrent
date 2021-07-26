// By Mike Taghavi ( @mitghi )
// for AARCH64
//

	.text
	.p2align 2
	.global concurrent_arch_setup_execution_context
	.global concurrent_arch_trampoline_to_procedure
	.global concurrent_arch_trampoline_to_caller
	.global concurrent_offsetof_procedure
	.global concurrent_offsetof_stack_ptr
	.global concurrent_offsetof_caller_return_addr

concurrent_arch_setup_execution_context:

	ldr	x6, =concurrent_offsetof_stack_ptr
	ldr	x6, [x6]
	add 	x5, x0, x6
	ldr	x3, [x5]
	mov	x2, sp
	mov	sp, x3

	ldr	x6, =concurrent_offsetof_procedure

	ldr	x6, [x6]
	add	x5, x0, x6
	ldr 	x4, [x5]
	mov 	x3, x4

	eor 	x6, x6, x6
	stp 	x0, x0, [sp, #-16]!
	stp	x6, x6, [sp, #-16]!
	stp	x6, x6, [sp, #-16]!
	stp	x6, x6, [sp, #-16]!
	stp	x6, x6, [sp, #-16]!
	stp	x6, x6, [sp, #-16]!	
	stp 	x2, x3, [sp, #-16]!

	stp 	q0, q1, [sp, #-32]!
	stp 	q2, q3, [sp, #-32]!
	stp 	q4, q5, [sp, #-32]!
	stp 	q6, q7, [sp, #-32]!
	stp 	q8, q9, [sp, #-32]!
	stp 	q10, q11, [sp, #-32]!
	stp 	q12, q13, [sp, #-32]!
	stp 	q14, q15, [sp, #-32]!
	stp 	q16, q17, [sp, #-32]!
	stp 	q18, q19, [sp, #-32]!
	stp 	q20, q21, [sp, #-32]!
	stp 	q22, q23, [sp, #-32]!
	stp 	q24, q25, [sp, #-32]!
	stp 	q26, q27, [sp, #-32]!
	stp 	q28, q29, [sp, #-32]!
	stp 	q30, q31, [sp, #-32]!

	ldr	x6, =concurrent_offsetof_stack_ptr

	ldr	x6, [x6]
	add 	x5, x0, x6
	mov 	x4, sp
	str 	x4, [x5]

	mov 	sp, x2
	ret

concurrent_arch_trampoline_to_procedure:
	stp 	x19, x20, [sp, #-16]!
	stp 	x21, x22, [sp, #-16]!
	stp 	x23, x24, [sp, #-16]!
	stp 	x25, x26, [sp, #-16]!
	stp 	x27, x28, [sp, #-16]!
	stp 	x29, x30, [sp, #-16]!

	stp 	q0, q1, [sp, #-32]!
	stp 	q2, q3, [sp, #-32]!
	stp 	q4, q5, [sp, #-32]!
	stp 	q6, q7, [sp, #-32]!
	stp 	q8, q9, [sp, #-32]!
	stp 	q10, q11, [sp, #-32]!
	stp 	q12, q13, [sp, #-32]!
	stp 	q14, q15, [sp, #-32]!
	stp 	q16, q17, [sp, #-32]!
	stp 	q18, q19, [sp, #-32]!
	stp 	q20, q21, [sp, #-32]!
	stp 	q22, q23, [sp, #-32]!
	stp 	q24, q25, [sp, #-32]!
	stp 	q26, q27, [sp, #-32]!
	stp 	q28, q29, [sp, #-32]!
	stp 	q30, q31, [sp, #-32]!


	ldr	x6, =concurrent_offsetof_stack_ptr

	ldr	x6, [x6]
	add 	x5, x0, x6
	ldr 	x3, [x5]
	ldr	x6, =concurrent_offsetof_procedure

	ldr	x6, [x6]
	add 	x6, x0, x6
	ldr 	x2, [x6]
	mov 	x7, sp
	str 	x7, [x5]
	mov 	sp, x3


	ldr 	x4, =concurrent_arch_return_at_procedure

	mov 	lr, x4


	ldp 	q30, q31, [sp], #32
	ldp 	q28, q29, [sp], #32
	ldp 	q26, q27, [sp], #32
	ldp 	q24, q25, [sp], #32
	ldp 	q22, q23, [sp], #32
	ldp 	q20, q21, [sp], #32
	ldp 	q18, q19, [sp], #32
	ldp 	q16, q17, [sp], #32
	ldp 	q14, q15, [sp], #32
	ldp 	q12, q13, [sp], #32
	ldp 	q10, q11, [sp], #32
	ldp 	q8, q9, [sp], #32
	ldp 	q6, q7, [sp], #32
	ldp 	q4, q5, [sp], #32
	ldp 	q2, q3, [sp], #32
	ldp 	q0, q1, [sp], #32


	ldp 	x29, x3, [sp], #16
	ldp 	x27, x28, [sp], #16
	ldp 	x25, x26, [sp], #16
	ldp 	x23, x24, [sp], #16
	ldp 	x21, x22, [sp], #16
	ldp 	x19, x20, [sp], #16

	br 	x3


concurrent_arch_trampoline_to_caller:
	stp 	x19, x20, [sp, #-16]!
	stp 	x21, x22, [sp, #-16]!
	stp 	x23, x24, [sp, #-16]!
	stp 	x25, x26, [sp, #-16]!
	stp 	x27, x28, [sp, #-16]!
	stp 	x29, x30, [sp, #-16]!

	stp 	q0, q1, [sp, #-32]!
	stp 	q2, q3, [sp, #-32]!
	stp 	q4, q5, [sp, #-32]!
	stp 	q6, q7, [sp, #-32]!
	stp 	q8, q9, [sp, #-32]!
	stp 	q10, q11, [sp, #-32]!
	stp 	q12, q13, [sp, #-32]!
	stp 	q14, q15, [sp, #-32]!
	stp 	q16, q17, [sp, #-32]!
	stp 	q18, q19, [sp, #-32]!
	stp 	q20, q21, [sp, #-32]!
	stp 	q22, q23, [sp, #-32]!
	stp 	q24, q25, [sp, #-32]!
	stp 	q26, q27, [sp, #-32]!
	stp 	q28, q29, [sp, #-32]!
	stp 	q30, q31, [sp, #-32]!

	ldr	x6, =concurrent_offsetof_stack_ptr

	ldr	x6, [x6]
	add 	x5, x0, x6
	ldr 	x3, [x5]
	mov 	x4, sp
	str 	x4, [x5]
	mov 	sp, x3

	ldp 	q30, q31, [sp], #32
	ldp 	q28, q29, [sp], #32
	ldp 	q26, q27, [sp], #32
	ldp 	q24, q25, [sp], #32
	ldp 	q22, q23, [sp], #32
	ldp 	q20, q21, [sp], #32
	ldp 	q18, q19, [sp], #32
	ldp 	q16, q17, [sp], #32
	ldp 	q14, q15, [sp], #32
	ldp 	q12, q13, [sp], #32
	ldp 	q10, q11, [sp], #32
	ldp 	q8, q9, [sp], #32
	ldp 	q6, q7, [sp], #32
	ldp 	q4, q5, [sp], #32
	ldp 	q2, q3, [sp], #32
	ldp 	q0, q1, [sp], #32

	ldp 	x29, x30, [sp], #16
	ldp 	x27, x28, [sp], #16
	ldp 	x25, x26, [sp], #16
	ldp 	x23, x24, [sp], #16
	ldp 	x21, x22, [sp], #16
	ldp 	x19, x20, [sp], #16

	br 	x30

concurrent_arch_return_at_procedure:
	ldp 	x0, x1, [sp], #16

	ldr	x6, =concurrent_offsetof_stack_ptr

	ldr	x6, [x6]
	add 	x5, x0, x6
	ldr 	x3, [x5]
	mov 	sp, x3

	ldp 	q30, q31, [sp], #32
	ldp 	q28, q29, [sp], #32
	ldp 	q26, q27, [sp], #32
	ldp 	q24, q25, [sp], #32
	ldp 	q22, q23, [sp], #32
	ldp 	q20, q21, [sp], #32
	ldp 	q18, q19, [sp], #32
	ldp 	q16, q17, [sp], #32
	ldp 	q14, q15, [sp], #32
	ldp 	q12, q13, [sp], #32
	ldp 	q10, q11, [sp], #32
	ldp 	q8, q9, [sp], #32
	ldp 	q6, q7, [sp], #32
	ldp 	q4, q5, [sp], #32
	ldp 	q2, q3, [sp], #32
	ldp 	q0, q1, [sp], #32


	ldp 	x29, x30, [sp], #16
	ldp 	x27, x28, [sp], #16
	ldp 	x25, x26, [sp], #16
	ldp 	x23, x24, [sp], #16
	ldp 	x21, x22, [sp], #16
	ldp 	x19, x20, [sp], #16

	ret

	.comm   concurrent_offsetof_procedure,8,3
	.comm   concurrent_offsetof_stack_ptr,8,3
	.comm   concurrent_offsetof_caller_return_addr,8,3
