/* Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Pat Beirne <patb@corelcomputer.com>

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* clone() is even more special than fork() as it mucks with stacks
   and invokes a function in the right context after its all over.  */

#define __ASSEMBLY__
#include <asm/errno.h>
#include <sys/syscall.h>

/* int clone(int (*fn)(void *arg), void *child_stack, int flags, void *arg); */

.text
.globl __clone;
.type __clone,@function
.align 4;
.globl ARC_SYMBOL_NAME(clone)
	ARC_SYMBOL_NAME(clone) = __clone

#ifdef __CONFIG_ARCH_ARC_A7__

__clone:
	cmp r0, 0	;No Null function pointers
	cmp.ne r1, 0	; No Null stack pointers
	mov.z r0, -EINVAL	; Invalid operands
	bz.nd __syscall_error_lcl

	; insert the args into the new stack
	

	;Save the fn pointer and the arg pointer
	mov	r10,r0
	mov	r11,r3
	;put parameters in place for sys_clone_wrapper
	mov 	r0,r2
	mov r8, __NR_clone	; save the syscall type in r0

	; do the syscall
	trap0                   ; do the interrupt (syscall)

	sub.f	0, r0, 0	; Check if some error occured in the syscall
	nop			; flush the pipeline

	blt	__syscall_error_lcl ; error 
	
	jnz.nd	[blink]		; return to parent

	;return to child
	mov	r0,r11
	mov	blink,r10
	j.nd	[blink]

__syscall_error_lcl:
#ifndef __PIC__	
    ; (1) No need to make -ve kernel error code as positive errno
    ;   __syscall_error expects the -ve error code returned by kernel
    ; (2) r0 still had orig -ve kernel error code
    ; (3) Tail call to __syscall_error so we dont have to come back here
    ;   hence instead of jmp-n-link we do jmp
    ;   A jl would have required push/pop blink here as well
    b  __syscall_error
#else
    b   ARC_SYMBOL_NAME(__syscall_error)@plt
#endif

.globl _clone
	_clone = __clone
;--------------------------------------------------------------
#else /* CONFIG_ARCH_ARC_A5   or A4 */

__clone:
	; sanity-check arguments
#ifdef __CONFIG_ARCH_ARC_A4__
	sub.f 0, r0, 0		; See if they passed a NULL function.
	nop			; Have to put a nop after an inst. with .f
	sub.nz.f 0, r1, 0	; See if they passed a NULL addr for the stack space
	nop
#elif __CONFIG_ARCH_ARC_A5__
	cmp r0, 0
	cmp.ne r1, 0
#else 
#error Unknown ARCTangent Archtitecture
#endif
	mov.z r0, -EINVAL	; Invalid operands
	bz.nd __syscall_error_lcl

	; insert the args into the new stack
	
	;save the fn ptr and arg ptr
	mov	r10,r0
	mov	r11,r3
1:	
	lr     r0, [AUX_IRQ_HINT]
	
	mov.f  0, r0
	nop
	bnz    1b

	;Put parameters in place for sys_clone	
	mov r0, r2
	mov r2,	r1
	mov r1,	r0
	mov r0, __NR_clone	; save the syscall type in r0

	; do the syscall
	sr SYSCALL_IRQ, [AUX_IRQ_HINT] ; do the interrupt (syscall)
	nop
	nop

	sub.f	0, r0, 0	; Check if some error occured in the syscall
	nop			; flush the pipeline
	blt	__syscall_error_lcl ; error 
	jne.nd	[blink]		; return to parent
	
	;return to child
	mov	r0,r11
	mov	blink,r10
	j.nd	[blink]

;#endif
	

__syscall_error_lcl:
	; set the errno
#ifdef __CONFIG_ARCH_ARC_A5
	rsub	r2, r0, 0	;  get -r0 in r2 (which will give us +errno)
#else
;	sub	r2,0,r0	
#endif
	st	r2, [ARC_SYMBOL_NAME(errno)]

#ifdef __CONFIG_ARCH_ARC_A4__
	mov	r0, -1
	j.nd blink
#else
	/* return -1 */
	j.d	[blink]
	mov	r0, -1
#endif


#endif /* CONFIG_ARCH_ARC_A7 */
