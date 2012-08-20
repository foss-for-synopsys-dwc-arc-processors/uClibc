/* Wrapper for setting errno.
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>
#include <features.h>

/* This routine is jumped to by all the syscall handlers, to stash
 * an error number into errno.
 *
 * vineetg: Dec 4th 2009
 * --Copied from ARM
 * --Had to remove the hiddn attribute. This is because other libs such as
 *   librt use the sys call wrappers and thus need to jump to it.
 *   Either we build this file with all external libs or just leave it
 *   exposed here.
 *   Someone with more uClibc wisdom shall fix it eventually
 */
//int __syscall_error(int err_no) attribute_hidden;
int __syscall_error(int err_no)
{
	__set_errno(-err_no);
	return -1;
}
