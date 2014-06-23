/*
 * Thread-local storage handling in statically linked binaries.
 * Copyright (C) 2009 Free Software Foundation, Inc.
 *
 * Based on GNU C Library (file: libc/sysdeps/sh/libc-tls.c)
 *
 * Copyright (C) 2013 Synopsys, Inc. (www.synopsys.com)
 * Copyright (C) 2010 STMicroelectronics Ltd.
 *
 * Author: Filippo Arcidiacono <filippo.arcidiacono@st.com>
 *
 * Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.
 */

#include <sysdeps/generic/libc-tls.c>
#include <dl-tls.h>

#if defined(USE_TLS) && USE_TLS

/* This minimal implementation is for static use only.  Because the linker
   eliminates dynamic tls accesses in a static link (and in the descriptor
   model, they don't directly involve __tls_get_addr in the first place),
   this code should actually be dead now.
   The full-featured implmenentation is found in ldso/ldso/dl-tls.c, and
   forms part of ld-uClibc*.so .  */
void *
__tls_get_addr (GET_ADDR_ARGS)
{
  dtv_t *dtv = THREAD_DTV ();
  return (char *) dtv[1].pointer.val + _ti_offset;
}

#endif
