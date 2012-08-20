#include "newlib/asm.h"

#undef ENDFUNC
#define ENDFUNC(name) ENDFUNC0(name) ` libc_hidden_def(name)
