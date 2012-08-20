/* brk system call for Linux/ARC.
   Copyright (C) 1995, 1996 Free Software Foundation, Inc.
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
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

libc_hidden_proto(brk)

/* This must be initialized data because commons can't have aliases.  */
//void *___brk_addr = 0;
void *__curbrk = 0;

int brk (void *addr)
{
	void *newbrk;


#ifdef __CONFIG_ARCH_ARC_A7__
	__asm__ (
		/* FIXME: do we need to save the sp here? */
		"mov   r0, %1 \n\t"   /* save the argment in r1 */
		"mov   r8, %2 \n\t"   /* and the syscall number in r8 */

		"trap0 \n\t" /* do the interrupt (syscall) */
		"nop   \n\t"          /* two nops to flush pipeline (hw issue) */
		"nop   \n\t"
		"mov %0, r0;"	/* keep the return value */

		: "=r"(newbrk) 
		: "r" (addr), "i" (__NR_brk)
		: "r0", "r8");

#else
	/* First, check if the AUX_HINT_REG is 0... ie, no interrupt
	 * is currently pending/being serviced 
	 */
	__asm__ (
		"1:    \n\t"
		"lr    r0, [%4] \n\t"
		"mov.f 0, r0 \n\t"
		"nop   \n\t"
		"bnz   1b \n\t"

		/* FIXME: do we need to save the sp here? */
		"mov   r1, %1 \n\t"   /* save the argment in r1 */
		"mov   r0, %2 \n\t"   /* and the syscall type in r0 */

		"sr    %3, [%4] \n\t" /* do the interrupt (syscall) */
		"nop   \n\t"          /* two nops to flush pipeline (hw issue) */
		"nop   \n\t"

		"mov.f 0, r0 \n\t"
		"nop   \n\t"

		"mov %0, r0;"	/* keep the return value */

		: "=r"(newbrk) 
		: "r" (addr), "i" (__NR_brk),
		  "i" (SYSCALL_IRQ), "i" (AUX_IRQ_HINT)
		: "r0", "r1");

	
#endif /* __CONFIG_ARCH_ARC_A7__ */

	__curbrk = newbrk;

	if (newbrk < addr) {
        return __syscall_error(-ENOMEM);
	}

    return 0;
}
libc_hidden_def(brk)
