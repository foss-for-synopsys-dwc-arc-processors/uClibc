/*
 * mmap() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* vineetg: Dec 2nd 2009
 *
 * switching from generic mmap implementation to ARC specific one.
 * Generic ver does old style mmap __NR_mmap with 1 packed buffer constaining
 * the 6 args and offset is in byte units.
 *
 * ARC Linux kernel upto (including) 2.6.30 support two mmap syscall
 * __NR_mmap => 1 packed arg and @offset in byte units
 * __NR_mmap2 => 6 args and @pgoffset in PAGE sized units
 *
 * Note that uClibc has an option __UCLIBC_MMAP_HAS_6_ARGS__ to do old mmap
 * with with 6 args but ARC Linux doesnt support it, so I didn't bother to
 * implement that case here
 */

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <bits/uClibc_page.h>  // MMAP2_PAGE_SHIFT

#define __NR__mmap __NR_mmap2

static inline _syscall6(__ptr_t, _mmap, void *, start, size_t, length,
		int, prot, int, flags, int, fd, off_t, offset);

__ptr_t mmap(__ptr_t addr, size_t len, int prot,
             int flags, int fd, __off_t offset)
{
  /* check if offset is page aligned */
    if (offset & ((1 << MMAP2_PAGE_SHIFT) - 1))
        return (__ptr_t) __syscall_error(-EINVAL);

    return (__ptr_t) _mmap (addr, len, prot, flags, fd,
			    /* Be careful: OFFSET is off_t, which is naturally
			       signed and will make the shift result get
			       sign-extended.  */
			    (((__u_long) offset) >> MMAP2_PAGE_SHIFT));
}

libc_hidden_def(mmap)
