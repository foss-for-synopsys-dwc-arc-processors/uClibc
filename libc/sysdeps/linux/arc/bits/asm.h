#ifndef __ARCH_ARC_INC_MACHINE_ASM
#define __ARCH_ARC_INC_MACHINE_ASM

#define _ENTRY(name)	.text ` .align 4 ` .globl name ` name:
#define __FUNC(name)	.type name,@function
#define ENTRY(name)	_ENTRY(name) ` __FUNC(name)

#define _END(name)	.size name,.-name
#define ENDFUNC(name)	_END(name) ` libc_hidden_def(name)

/* Pos-independent code requires funcs be called via PLT
 * when building with -fPIC, gcc driver automagically #defs __PIC__
 */
#ifdef __PIC__
#define PIC_SYM(x) x@plt
#else
#define PIC_SYM(x) x
#endif


#endif
