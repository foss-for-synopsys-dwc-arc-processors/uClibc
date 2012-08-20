/* Machine-dependent pthreads configuration and inline functions.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */
#ifndef _PT_MACHINE_H
#define _PT_MACHINE_H   1
#include <features.h>

#ifndef PT_EI
# define PT_EI __extern_always_inline /* ARC LOCAL: was: extern inline */
#endif

extern long int testandset (int *spinlock);
extern int __compare_and_swap (long int *p, long int oldval, long int newval);

PT_EI long int
testandset (int *spinlock)
{
  register unsigned long int temp,old;
  long flags;
  int *m = spinlock;
  
#if defined __CONFIG_ARCH_ARC_A7__
  __asm__ ("ex %0,[%3]" : "=r" (old), "+m" (*m) : "0" (1), "ri" (m));
  return old;
#else
  /* Note the use of a dummy output of *spinlock to expose the write.  The
     memory barrier is to stop *other* writes being moved past this code.  */
  save_flags(flags); cli();
#if defined __CONFIG_ARCH_ARC_A5
  
  
  __asm__ __volatile__(
       "	ld %2,%3\n"
       "	bset  %0,%2,0\n"
       "	st %0,%1"
       :"=&r" (temp), "=m" (*m), "=&r" (old)
       : "m" (*m));
  
#else
  old = (long int)*m;
  if(!(*m & 1))
	  *m  = *m | 1;
#endif
  restore_flags(flags);
  return (old & 1 ) != 0;
#endif

}

/* Get some notion of the current stack.  Need not be exactly the top
   of the stack, just something somewhere in the current frame.
   I don't trust register variables, so let's do this the safe way.  */
#define CURRENT_STACK_FRAME \
({ char *sp; __asm__ ("mov %0,sp" : "=r" (sp)); sp; })

#else
#error PT_MACHINE already defined
#endif /* pt-machine.h */
