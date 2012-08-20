/******************************************************************************
 * Copyright Synopsys, Inc. (www.synopsys.com) Oct 01, 2004
 * 
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 *****************************************************************************/

/* Define the __set_errno macro as nothing so that INLINE_SYSCALL
 * won't set errno, which is important since we make system calls
 * before the errno symbol is dynamicly linked.

#define __set_errno(X) {(void)(X);}
*/

/* vineetg: Dec 1st 2009:
 * Instead of doing inline errno setting with __set_errno, the sys-call
 * macros now do it in a external function __syscall_error to reduce the
 * code in sys-call wrapper itself.
 * Thus ldso needs a equivalent def
 */
#define __syscall_error(X) (X)

#include "sys/syscall.h"
#include <bits/uClibc_page.h>
