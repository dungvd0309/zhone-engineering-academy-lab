	.file	"lab_6.c"
	.text
	.globl	add
	.type	add, @function
add:
.LFB0:
	# add Prologue
	pushq	%rbp
	movq	%rsp, %rbp
	
	# Save the register-passed arguments to the stack
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)

	# Perform the addition
	movq	-8(%rbp), %rdx # move a to rdx
	movq	-16(%rbp), %rax # move b to rax
	addq	%rdx, %rax 		# add a + b, result in rax
	
	# add Epilogue
	popq	%rbp
	ret
.LFE0:
	.size	add, .-add
	.globl	myfunc
	.type	myfunc, @function

myfunc:
.LFB1:

	# myfunc Prologue
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp

	# Save the register-passed arguments to the stack
	movq	%rdi, -24(%rbp) # a
	movq	%rsi, -32(%rbp) # b
	movq	%rdx, -40(%rbp) # c
	movq	%rcx, -48(%rbp) # d
	movq	%r8, -56(%rbp)  # e
	movq	%r9, -64(%rbp)  # f

	# Perform the addition
	movq	-24(%rbp), %rdx # move a to rdx
	movq	-32(%rbp), %rax # move b to rax
	addq	%rax, %rdx		# add a + b, result in rdx

	movq	-40(%rbp), %rax # add c
	addq	%rax, %rdx

	movq	-48(%rbp), %rax # add d
	addq	%rax, %rdx

	movq	-56(%rbp), %rax # add e
	addq	%rax, %rdx

	movq	-64(%rbp), %rax # add f
	addq	%rax, %rdx

	movq	16(%rbp), %rax 	# add g (already on the stack)
	addq	%rax, %rdx

	movq	24(%rbp), %rax 	# add h (already on the stack)
	addq	%rdx, %rax

	movq	%rax, -16(%rbp) # Store the result 
							# in a local variable xx
	movq	-16(%rbp), %rax

	# Register-passed argument for add function
	movl	$10, %esi
	movq	%rax, %rdi

	# Save the return address and call add function
	call	add

	movq	%rax, -8(%rbp) 	# Store the result of add 
							# in a local variable yy
	movq	-8(%rbp), %rax 
	addq	$20, %rax # Add 20 to rax for the final result

	# myfunc Epilogue
	leave 
	ret

.LFE1:
	.size	myfunc, .-myfunc
	.globl	main
	.type	main, @function

main:
.LFB2:
	# main Prologue
	pushq	%rbp 		# save the old base pointer
	movq	%rsp, %rbp 	# set the new base pointer	
	subq	$16, %rsp 	# allocate space on the stack

	# Stack-passed arguments
	pushq	$8 
	pushq	$7
	# Register-passed arguments
	movl	$6, %r9d 
	movl	$5, %r8d
	movl	$4, %ecx
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi

	# Save the return address and call myfunc
	call	myfunc 

	# Clean up the stack after the function call
	addq	$16, %rsp
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax

	# main Epilogue
	leave 
	ret

.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
