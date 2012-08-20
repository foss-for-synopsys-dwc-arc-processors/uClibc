/* vfork for uClibc/ARC
 *
 * Copyright (C) 2000 by Lineo, inc. and Erik Andersen
 * Copyright (C) 2000,2001 by Erik Andersen <andersen@uclibc.org>
 * Copyright (C) 2003 by Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#define __ASSEMBLY__
#include <asm/errno.h>
/* 7-Jan-12: Jeremy Bennett <jeremy.bennett@embecosm.com>. This is a
  	     non-existant header, so removed. */
/* #include <asm/system.h> */
#include <asm/unistd.h>
#include <sys/syscall.h>

.text
; .extern _errno;
	.globl __vfork
	.hidden __vfork
	.type   __vfork,@function
	.align 4;

#ifdef __CONFIG_ARCH_ARC_A7__
ARC_SYMBOL_NAME(__vfork):		
	mov r8, __NR_vfork
	trap0
	sub.f 0, r0, 0
	jge [blink]

__syscall_error_lcl:

    ; ----- Errno related processing -------
    ; (1) No need to make -ve kernel error code as positive errno
    ;   __syscall_error expects the -ve error code returned by kernel
    ; (2) r0 still had orig -ve kernel error code
    ; (3) Tail call to __syscall_error: 
    ;       hence jmp-n-link (jl) instead of jmp (j)
    ;       That way __syscall_error( ) returns to caller of vfork
    ;       Also jl would have required push/pop blink around it too

#ifndef __PIC__
    b  __syscall_error
#else
    b   ARC_SYMBOL_NAME(__syscall_error)@plt
#endif

;---------------------------------------------------------------
#else
ARC_SYMBOL_NAME(__vfork):
#ifdef __NR_vfork
1:
	lr     r0, [AUX_IRQ_HINT]
	mov.f  0, r0
	nop
	bnz    1b

	; FIXME: do we need to save the sp here?
	mov r0, __NR_vfork	; save the syscall type in r0

	sr SYSCALL_IRQ, [AUX_IRQ_HINT] ; do the interrupt (syscall)
	nop                     ;  two nops to flush pipeline (hw issue)
	nop

	sub.f 0, r0, 0  	; Check if some error occured in the syscall
	nop			; flush the pipeline (make sure flags are set... hw bug)
	jge.nd [blink]
/*go_on:
	; Some error occured...

	; Check if vfork exists. it not, use fork.
	mov	r1, -ENOSYS
	sub.f	0, r0, r1*/
	nop
	blt.nd	__syscall_error_lcl
#endif

2:
/*	lr     r0, [AUX_IRQ_HINT]
	mov.f  0, r0
	nop
	bnz    2b

	; FIXME: do we need to save the sp here?
	mov r0, __NR_fork	; save the syscall type in r0

	sr SYSCALL_IRQ, [AUX_IRQ_HINT] ; do the interrupt (syscall)
	nop
	nop

	sub.f	0, r0, 0 	; Check if some error occured in the syscall
	nop			; flush the pipeline
	jge	[blink]		; done...
	*/

__syscall_error_lcl:
	; set the errno
	sub	r2, 0, r0	;  get -r0 in r2 (which will give us +errno)

	st	r2, [ARC_SYMBOL_NAME(errno)]

	mov	r0, -1
	j.nd [blink]

#endif /* CONFIG_ARCH_ARC_A7 */
.size __vfork,.-__vfork
weak_alias(__vfork,vfork)
libc_hidden_weak(vfork)

