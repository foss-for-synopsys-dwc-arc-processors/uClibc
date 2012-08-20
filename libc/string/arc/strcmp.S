#include "uclibc-asm.h"
#include <features.h>
#include "newlib/strcmp.S"

#ifndef __UCLIBC_HAS_LOCALE__
strong_alias(strcmp,strcoll)
libc_hidden_def(strcoll)
#endif
