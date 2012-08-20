/* setjmp for ARC, ELF version.
   Copyright (C) 1995, 1996, 1997, 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

;vineetg: Aug 21st 2009.
;   Old code was saving r4-r9 which seems non-sense per current ABI
;   as we want to save call-saved regs which is r13 to r25.

#define __ASSEMBLY__
#define _ASM
#define _SETJMP_H
#include <bits/setjmp.h>
#include <sys/syscall.h>

.globl ___sigsetjmp;
.type   ___sigsetjmp,@function
.align 4;

;@ r0 = jump buffer into which regs will be saved
;@ r1 = do we need to save signals

___sigsetjmp:

    st_s    r13,  [r0]
    st_s    r14,  [r0,4]
    st      r15,  [r0,8]
    st      r16,  [r0,12]
    st      r17,  [r0,16]
    st      r18,  [r0,20]
    st      r19,  [r0,24]
    st      r20,  [r0,28]
    st      r21,  [r0,32]
    st      r22,  [r0,36]
    st      r23,  [r0,40]
    st      r24,  [r0,44]
    st      r25,  [r0,48]

    st      fp,   [r0,52]
    st      sp,   [r0,56]

    ; make a note of where longjmp will return to.
    ; that will be right next to this setjmp call-site which will be contained
    ; in blink, since "C" caller of this routine will do a branch-n-link

    st      blink, [r0,60]

    b.nd    ARC_SYMBOL_NAME(__sigjmp_save)

.size ___sigsetjmp,.-___sigsetjmp

.globl ARC_SYMBOL_NAME(__sigsetjmp)
    ARC_SYMBOL_NAME(__sigsetjmp) = ___sigsetjmp
