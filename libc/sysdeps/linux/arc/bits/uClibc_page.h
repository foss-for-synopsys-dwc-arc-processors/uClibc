/******************************************************************************
 * Copyright Synopsys, Inc. (www.synopsys.com) May 12, 2005
 * 
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 *****************************************************************************/

/*
 *  sysdeps/linux/arc/bits/uClibc_page.h
 *
 *  Copyright (C) 
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 * Authors: Amit Bhor
 */
#ifndef _UCLIBC_PAGE_H
#define _UCLIBC_PAGE_H

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT	13
#ifdef __ASSEMBLY__
#define PAGE_SIZE       (1 << PAGE_SHIFT)
#else
#define PAGE_SIZE       (1UL << PAGE_SHIFT)
#endif	/* __ASSEMBLY */
#define PAGE_MASK       (~(PAGE_SIZE-1))

#define MMAP2_PAGE_SHIFT PAGE_SHIFT

/* for ldso */
#define ADDR_ALIGN      (PAGE_SIZE -1)
#define PAGE_ALIGN      PAGE_MASK
#define OFFS_ALIGN      PAGE_MASK

#endif /* _UCLIBC_PAGE_H */
