/* Copyright (C) 1997-2000,2002,2003,2005,2006 Free Software Foundation, Inc.
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
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>
#include <bits/kernel_sigaction.h>

extern __typeof(sigaction) __libc_sigaction;

extern void __default_rt_sa_restorer(void);
// TBD: 0.9.32 stable material
//libc_hidden_proto(__default_rt_sa_restorer);

#define SA_RESTORER	0x04000000

/* If ACT is not NULL, change the action for SIG to *ACT.
   If OACT is not NULL, put the old action for SIG in *OACT.  */
int
__libc_sigaction (int sig, const struct sigaction *act, struct sigaction *oact)
{
	int result;
	struct sigaction kact;

	if (act) {
		memcpy(&kact, act, sizeof(kact));

		if (!(kact.sa_flags & SA_RESTORER)) {
			kact.sa_restorer = __default_rt_sa_restorer;
			kact.sa_flags |= SA_RESTORER;
		}
	}

	/* XXX The size argument hopefully will have to be changed to the
	   real size of the user-level sigset_t.  */
	result = __syscall_rt_sigaction(sig,
					act ? &kact : NULL, oact,
					_NSIG / 8);

	return result;
}

#ifndef LIBC_SIGACTION
weak_alias(__libc_sigaction,sigaction)
libc_hidden_weak(sigaction)
#endif
