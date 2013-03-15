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

#include <sys/syscall.h>
#include <bits/asm.h>

#ifndef __NR_vfork	/* in absence of legacy vfork, emulate with clone */
#define _SIGNAL_H
#include <bits/signum.h>       /* For SIGCHLD */

#define CLONE_VM		0x00000100
#define CLONE_VFORK		0x00004000
#define CLONE_FLAGS_FOR_VFORK	(CLONE_VM|CLONE_VFORK|SIGCHLD)
#endif

ENTRY(vfork)
#ifdef __NR_vfork
	mov r8, __NR_vfork
#else
	mov r8, __NR_clone
	mov r0, CLONE_FLAGS_FOR_VFORK
	mov_s r1, sp
	mov_s r2, 0
	mov   r3, 0
	mov   r4, 0
#endif
	trap0

	sub.f 0, r0, 0
	jge [blink]

	b   PIC_SYM(__syscall_error)
ENDFUNC(vfork)
