/*
 * Copyright (C) 2022, Synopsys, Inc. (www.synopsys.com)
 *
 * Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.
 */

#ifndef _ARC_ASM_H
#define _ARC_ASM_H

#if defined (__ARC64_ARCH32__)

.macro PUSHR   r
  push	\r
.endm

.macro PUSHR_S   r
  push	\r
.endm

.macro POPR r
  pop	\r
.endm

.macro POPR_S r
  pop	\r
.endm

.macro ADDR_S r1, r2, v
  add	\r1, \r2, \v
.endm

#elif defined (__ARC64_ARCH64__)

# error ARCv3 64-bit is not supported by uClibc-ng

#else /* ARCHS || ARC700 */

.macro PUSHR   r
  push	\r
.endm

.macro PUSHR_S   r
  push_s	\r
.endm

.macro POPR   r
  pop	\r
.endm

.macro POPR_S   r
  pop_s	\r
.endm

.macro ADDR_S r1, r2, v
  add_s	\r1, \r2, \v
.endm

#endif

#endif /* _ARC_ASM_H  */
