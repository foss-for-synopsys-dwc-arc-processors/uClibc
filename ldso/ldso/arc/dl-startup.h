/*
 * Architecture specific code used by dl-startup.c
 *
 * Copyright Synopsys, Inc. (www.synopsys.com) Oct 01, 2004
 * Copyright (C) 2005 by Erik Andersen <andersen@codepoet.org>
 * Copyright (C) 2012 Synopsys, Inc. (www.synopsys.com)
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 */

#include <sys/syscall.h>

/* vineetg: Refactoring/cleanup of loader entry point
 *  Removed 6 useless insns
 * Joern Improved it even further:
 *  -better insn scheduling
 *  -no need for conditional code for _dl_skip_args
 *  -use of assembler .&2 expressions vs. @gotpc refs (avoids need for GP)
 *
 * What this code does:
 *  -ldso starts execution here when kernel returns from execve()
 *  -calls into generic ldso entry point _dl_start( )
 *  -optionally adjusts argc for executable if exec passed as cmd
 *  -calls into app main with address of finaliser
 */
__asm__(
    ".section .text                                             \n"
    ".balign 4                                                  \n"
    ".global  _start                                            \n"
    ".type  _start,@function                                    \n"

    "_start:                                                    \n"
    "   ; ldso entry point, returns app entry point             \n"
    "   bl.d    _dl_start                                       \n"
    "   mov_s   r0, sp          ; pass ptr to aux vector tbl    \n"

    "   ; If ldso ran as cmd with executable file nm as arg     \n"
    "   ; as arg, skip the extra args calc by dl_start()        \n"
    "   ld_s    r1, [sp]       ; orig argc from aux-vec Tbl     \n"
#ifdef STAR_9000535888_FIXED
    "   ld      r12, [pcl, _dl_skip_args-.+(.&2)]               \n"
#else
    "   add     r12, pcl, _dl_skip_args-.+(.&2)                 \n"
    "   ld      r12, [r12]                                      \n"
#endif

    "   add     r2, pcl, _dl_fini-.+(.&2)   ; finalizer         \n"

    "   add2    sp, sp, r12    ; discard argv entries from stack\n"
    "   sub_s   r1, r1, r12    ; adjusted argc, on stack        \n"
    "   st_s    r1, [sp]                                        \n"

    "   j_s.d   [r0]           ; app entry point                \n"
    "   mov_s   r0, r2         ; ptr to finalizer _dl_fini      \n"

    ".size  _start,.-_start                                     \n"
    ".previous                                                  \n"
);

/* we dont need to spit out argc, argv etc for debugging */
#define NO_EARLY_SEND_STDERR    1
