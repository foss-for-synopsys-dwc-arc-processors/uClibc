/*
 * cachectl.h - cacheflush routine
 *
 * Copyright (C) 2013 Synopsys, Inc. (www.synopsys.com)
 *
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

#ifndef _SYS_CACHECTL_H
#define _SYS_CACHECTL_H	1

/*
 * Get the kernel definition for the flag bits
 */
#include <asm/cachectl.h>

__BEGIN_DECLS

extern int cacheflush(void *addr, int nbytes, int flags);

__END_DECLS

#endif	/* sys/cachectl.h */
