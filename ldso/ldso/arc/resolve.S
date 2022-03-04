/*
 * Copyright (C) 2013 Synopsys, Inc. (www.synopsys.com)
 *
 * Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.
 */

#include <sysdep.h>
#include <sys/syscall.h>

; Save the registers which resolver could possibly clobber
; 	r0-r9: args to the function - symbol being resolved
; 	r10-r12 are already clobbered by PLTn, PLT0 thus neednot be saved

.macro	SAVE_CALLER_SAVED
#ifdef __ARC64_ARCH32__
	push	r0
	push	r1
	push	r2
	push	r3
#else
	push_s	r0
	push_s	r1
	push_s	r2
	push_s	r3
#endif
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
#ifdef __ARC64_ARCH32__
	push	blink
#else
	push_s	blink
#endif
.endm

.macro RESTORE_CALLER_SAVED_BUT_R0
	pop	blink
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
#ifdef __ARC64_ARCH32__
	pop	r3
	pop	r2
	pop	r1
#else
	pop_s	r3
	pop_s	r2
	pop_s	r1
#endif
.endm

; Upon entry, PLTn, which led us here, sets up the following regs
; 	r11 = Module info (tpnt pointer as expected by resolver)
;	r12 = PC of the PLTn itself - needed by resolver to find
;	      corresponding .rela.plt entry

ENTRY(_dl_linux_resolve)
	; args to func being resolved, which resolver might clobber
	SAVE_CALLER_SAVED

	mov_s 	r1, r12
	bl.d  	_dl_linux_resolver
	mov   	r0, r11

	RESTORE_CALLER_SAVED_BUT_R0
	j_s.d   [r0]    ; r0 has resolved function addr
#ifdef __ARC64_ARCH32__
	pop	r0      ; restore first arg to resolved call
#else
	pop_s	r0      ; restore first arg to resolved call
#endif
END(_dl_linux_resolve)
