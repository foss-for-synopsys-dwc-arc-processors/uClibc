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

#include <asm/errno.h>
#include <sys/syscall.h>
#include <bits/asm.h>

; Per man, libc clone( ) is as follows
;
; int clone(int (*fn)(void *), void *child_stack,
;           int flags, void *arg, ...
;           /* pid_t *ptid, struct user_desc *tls, pid_t *ctid */);
;
; NOTE: I'm assuming that the last 3 args are NOT var-args and in case all
;	3 are not relevant, caller will nevertheless pass those as NULL.
;       Current (Jul 2012) upstream powerpc/clone.S assumes similarly.
;	Our LTP (from 2007) doesn't seem to have tests to prove otherwise

; clone syscall in kernel
;
; int sys_clone(unsigned long clone_flags, unsigned long newsp,
;		int __user *parent_tidptr, void *tls,
;		int __user *child_tidptr)


ENTRY(clone)
	cmp	r0, 0		; @fn can't be NULL
	cmp.ne	r1, 0		; @child_stack can't be NULL
	mov.z	r0, -EINVAL
	bz	.L__sys_err

	; @fn and @args needed after the syscall for child
	; However r3 containing @arg will be clobbered BEFORE syscall
	; r0 containg @fn will be clobbered AFTER syscall (with ret val)
	mov	r10, r0
	mov	r11, r3

	; do the syscall
	mov 	r0, r2		; libc @flags
	mov	r2, r4		; libc @ptid
	mov	r3, r5		; libc @tls
	mov	r4, r6		; libc @ctid
	mov	r8, __NR_clone

	ARC_TRAP_INSN

	sub.f	0, r0, 0	; return code : 0 new process, !0 parent
	blt	.L__sys_err	; < 0 (signed) error
	jnz	[blink]		; Parent returns

	; child jumps off to @fn with @arg as argument
	j.d	[r10]
	mov	r0, r11

	; falls thru to _exit() with result from @fn (already in r0)
	b	HIDDEN_JUMPTARGET(_exit)

.L__sys_err:
	; (1) No need to make -ve kernel error code as positive errno
	;   __syscall_error expects the -ve error code returned by kernel
	; (2) r0 still had orig -ve kernel error code
	; (3) Tail call to __syscall_error so we dont have to come back
	;     here hence instead of jmp-n-link (reg push/pop) we do jmp
	b   PIC_SYM(__syscall_error)
ENDFUNC(clone)
