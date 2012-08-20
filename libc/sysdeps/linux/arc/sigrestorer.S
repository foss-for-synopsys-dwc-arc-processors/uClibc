/* Copyright (C) 1999 Free Software Foundation, Inc.
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

#define __ASSEMBLY__    /* kernel build system expects __ASSEMBLY__
                            uclibc uses __ASSEMBLER__
                         */
#include <sys/syscall.h>

/*  vineetg: July 2009
    uClibc to provide sigreturn stub, rather than kernel synthesising it on
    user stack at runtime, fiddlign with PTE/TLB flags and expensive
    Cache line flushes
*/
     
.globl __default_sa_restorer;
.type __default_sa_restorer, @function
.align 4;
__default_sa_restorer:
 	mov r8, __NR_sigreturn
    trap0
    nop
    nop

.size __default_sa_restorer, .-__default_sa_restorer
libc_hidden_def(__default_sa_restorer)

#ifdef __NR_rt_sigreturn

.globl __default_rt_sa_restorer;
.type __default_rt_sa_restorer, @function
.align 4;
__default_rt_sa_restorer:
 	mov r8, __NR_rt_sigreturn
    trap0
    nop
    nop
.size __default_rt_sa_restorer, .-__default_rt_sa_restorer
libc_hidden_def(__default_rt_sa_restorer)
#endif
