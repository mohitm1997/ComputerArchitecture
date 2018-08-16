.globl nCr
	.type	nCr, @function
#calculates the nCr, given ints n and r using the factorial function
nCr:
	pushl %ebp
	movl %esp, %ebp
	subl $12, %esp
	movl 8(%ebp), %eax
	movl %eax, (%esp)
	call Factorial
	cmpl $0, %eax
	je .L5
	movl %eax, -4(%ebp)
	movl 8(%ebp), %eax
	subl 12(%ebp), %eax
	movl %eax, (%esp)
	call Factorial
	cmpl $0, %eax
	je .L5
	movl %eax, -8(%ebp)
	movl 12(%ebp), %eax
	movl %eax, (%esp)
	call Factorial
	cmpl $0, %eax
	je .L5
	imull -8(%ebp), %eax
	movl %eax, -12(%ebp)
	movl -4(%ebp), %eax
	cltd
	idivl -12(%ebp)
	addl $12, %esp
	popl %ebp
	ret
.L5:
	addl $12, %esp
	popl %ebp
	ret
.globl Factorial
	.type	Factorial, @function
#calculates the factorial of the input
Factorial:
	pushl %ebp
	movl %esp, %ebp
	pushl %edx
	movl 8(%ebp), %edx
	movl $1, %eax
	cmpl $1, %edx
	jle .L3
.L2:
	imull %edx, %eax
	jo .L4
	subl $1, %edx
	cmpl $1, %edx
	jg .L2
.L3:
	popl %edx
	popl %ebp
	ret
.L4:
	movl $0, %eax
	popl %edx
	popl %ebp
	ret

