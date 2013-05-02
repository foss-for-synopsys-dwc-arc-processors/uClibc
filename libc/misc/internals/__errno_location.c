/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <errno.h>

#ifndef __UCLIBC_HAS_TLS__
# undef errno
extern int errno;
#endif

int *
#ifndef __UCLIBC_HAS_THREADS__
weak_const_function
#endif
__errno_location(void)
{
    return &errno;
}
libc_hidden_def(__errno_location)
