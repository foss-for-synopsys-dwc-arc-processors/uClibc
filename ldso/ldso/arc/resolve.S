/******************************************************************************
 * Copyright Synopsys, Inc. (www.synopsys.com) 2012
 *
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 *****************************************************************************/

/*
 * vineetg: Written - October 2010, applied April 2012
 *
 *  -Optimised the regs saved/restored across entry into resolver
 *   The only regs we need to save/resore are the ones which can potentially
 *   contain function args for the external function call.
 *   The orig code was saving r0-r15, r24, r25, blink) (total 19)
 *   whereas we need to save only r0-r9, blink (total 11)
 */

#define __ASSEMBLY__
 #include <sys/syscall.h>

; Save the registers which resolver could possibly clobber
; 	r0-r9: args to the function - symbol being resolved
; 	r10-r12 are already clobbered by PLTn, PLT0 thus neednot be saved

.macro	SAVE_CALLER_SAVED
	push_s	r0
	push_s	r1
	push_s	r2
	push_s	r3
	st.a	r4, [sp, -4]
	st.a	r5, [sp, -4]
	st.a	r6, [sp, -4]
	st.a	r7, [sp, -4]
	st.a	r8, [sp, -4]
	st.a	r9, [sp, -4]
	push_s	blink
.endm

.macro RESTORE_CALLER_SAVED_BUT_R0
	ld.ab	blink,[sp, 4]
	ld.ab	r9, [sp, 4]
	ld.ab	r8, [sp, 4]
	ld.ab	r7, [sp, 4]
	ld.ab	r6, [sp, 4]
	ld.ab	r5, [sp, 4]
	ld.ab	r4, [sp, 4]
	pop_s   r3
	pop_s   r2
	pop_s   r1
.endm

.text
.align 4

.globl __dl_linux_resolve
.globl _dl_linux_resolve
.type	_dl_linux_resolve,@function
.type  __dl_linux_resolve,@function

; Upon entry, PLTn, which led us here, sets up the following regs
; 	R11 = Module info (tpnt pointer as expected by resolver)
;	R12 = PC of the PLTn itself - needed by resolver to find
;		  corresponding .rela.plt entry
__dl_linux_resolve:
_dl_linux_resolve:
		; args to func being resolved, which resolver might clobber
		SAVE_CALLER_SAVED

		mov_s 	r1, r12
		bl.d  	ARC_SYMBOL_NAME(_dl_linux_resolver)
		mov   	r0, r11

		RESTORE_CALLER_SAVED_BUT_R0
		j_s.d   [r0]    ; r0 has resolved function addr
		pop_s   r0      ; restore first arg to resolved call
.LFE2:
	.size _dl_linux_resolve,.LFE2-_dl_linux_resolve

