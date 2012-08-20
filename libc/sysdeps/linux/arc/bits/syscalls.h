/* Syscall Macro Generator for ARC 
 * Originally written by Codeito, who in turn borrowed it from ARM/???
 *
 * vineet.gupta@viragelogic.com: Dec 1st 2009
 * Rewrote ARC700 syscall wrapper generator macro.
 *
 * 1. Got rid of all the superfluous register loads by specifying the args
 *    as __asm__("r-nn") as they are already in right order
 *
 * 2. Instead of updating errno inline, shunted it to a function to
 *    reduce code in EACH wrapper.
 *    push/pop for blink done only if branch actually taken.
 *
 * 3. Removed the 2 nop insns after swi as ARC700 doesn't have any such reqmt
 *
 * 4. No dependency on mar errno defined by kernel. A signed -ve number 
 *    (top bit set) is considered kernel returned error. 
 *    That way we dont need to follow kernel errno limits and also
 *    avoid a -ve long immediate from code (typically 4 bytes) 
 *
 * Note the memory clobber is not strictly needed for intended semantics of
 * the inline asm. However some of the cases, such as old-style 6 arg mmap
 * gcc was generating code for inline syscall ahead of buffer packing needed
 * for syscall itself.
 */
#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
#error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#include <features.h>
#include <bits/sysnum.h>
#include <asm/unistd.h>
/* 7-Jan-12: Jeremy Bennett <jeremy.bennett@embecosm.com>. This is a
  	     non-existant header, so removed. */
/* #include <asm/system.h> */
#ifndef __ASSEMBLER__
#include <errno.h>
#endif

#ifdef __ARCH_HAS_C_SYMBOL_PREFIX__
#define ARC_SYMBOL_NAME(x) _##x
#define ARC_SYMBOL_ALIAS(x) ARC_SYMBOL_NAME(x) = x
#else
#define ARC_SYMBOL_NAME(x) x
#define ARC_SYMBOL_ALIAS(x)
#endif

/* Sys call name to number */
#ifndef SYS_ify
#define SYS_ify(syscall_name)	(__NR_##syscall_name)
#endif

/*
   Some of the sneaky macros in the code were taken from 
   glibc-2.2.5/sysdeps/unix/sysv/linux/arm/sysdep.h
*/

#ifndef __ASSEMBLER__

/* * Conditional because ldso defines it as a macro  */
#ifndef __syscall_error
extern int __syscall_error(int err_no);
#endif

/*-------------------------------------------------------------------------
 * Core syscall wrapper for ARC: Common to ARC700 and 600
 *-------------------------------------------------------------------------*/

/* call for errno-setting seperated off to avoid push/pop unconditionally */

#ifdef IS_IN_rtld
#define ERRNO_ERRANDS(_sys_result) (0)
#else /* !IS_IN_rtld */
#ifdef __PIC__
#define CALL_ERRNO_SETTER   "bl   __syscall_error@plt    \n\t"
#else
#define CALL_ERRNO_SETTER   "bl   __syscall_error    \n\t"
#endif

#define ERRNO_ERRANDS(_sys_result)          \
        __asm__ volatile (                  \
        "st.a blink, [sp, -4] \n\t"         \
        CALL_ERRNO_SETTER                   \
        "ld.ab blink, [sp, 4] \n\t"         \
        :"+r" (_sys_result)                 \
        :                                   \
        :"r1","r2","r3","r4","r5","r6",     \
         "r7","r8","r9","r10","r11","r12"   \
        );

#endif /* IS_IN_rtld */

/* Invoke the syscall and return unprocessed kernel status */
#define INTERNAL_SYSCALL(nm, err, nr, args...)		\
	INTERNAL_SYSCALL_NCS(SYS_ify (nm), err, nr, args)

/* -1 to -1023 as valid error values will suffice for some time */
#define INTERNAL_SYSCALL_ERROR_P(val, err)		\
	((unsigned int) (val) > (unsigned int) -1024)

#define INTERNAL_SYSCALL_ERRNO(val, err)        (-(val))

#define INTERNAL_SYSCALL_DECL(err) 		do { } while (0)

/*
 * Standard sycall wrapper:
 *  -"const" syscall number @nm, sets errno, return success/error-codes
 */
#define INLINE_SYSCALL(nm, nr_args, args...)				\
({									\
	register int __res __asm__("r0");				\
	__res = INTERNAL_SYSCALL(nm, , nr_args, args);			\
	if (__builtin_expect (INTERNAL_SYSCALL_ERROR_P ((__res), ), 0))	\
	{								\
		ERRNO_ERRANDS(__res);					\
	}								\
	__res;								\
})

/* Non const syscall number @nm
 * Ideally this could be folded within INLINE_SYSCALL with
 * __builtin_constant_p in INTERNAL_SYSCALL but that fails for syscall.c
 */
#define INLINE_SYSCALL_NCS(nm, nr_args, args...)			\
({									\
	register int __res __asm__("r0");				\
	__res = INTERNAL_SYSCALL_NCS(nm, , nr_args, args);		\
	if (__builtin_expect (INTERNAL_SYSCALL_ERROR_P ((__res), ), 0))	\
	{								\
		ERRNO_ERRANDS(__res);					\
	}								\
	__res;								\
})

/*-------------------------------------------------------------------------
 * Mechanics of Trap - specific to ARC700
 *-------------------------------------------------------------------------*/

#ifdef __CONFIG_ARCH_ARC_A7__

#define INTERNAL_SYSCALL_NCS(nm, err, nr_args, args...)	\
({							\
	/* Per ABI, r0 is 1st arg and return reg */	\
	register int __ret __asm__("r0");		\
	register int _sys_num __asm__("r8") = (nm);	\
	LOAD_ARGS_##nr_args (__ret, args)		\
							\
        __asm__ volatile (				\
		"trap0          \n\t"			\
		: "+r" (__ret)				\
		: "r"(_sys_num) ASM_ARGS_##nr_args	\
		: "memory");				\
                                                        \
	__ret;						\
})

/* Macros for setting up inline __asm__ input regs */
#define ASM_ARGS_0
#define ASM_ARGS_1	ASM_ARGS_0, "r" (__ret)
#define ASM_ARGS_2	ASM_ARGS_1, "r" (_arg2)
#define ASM_ARGS_3	ASM_ARGS_2, "r" (_arg3)
#define ASM_ARGS_4	ASM_ARGS_3, "r" (_arg4)
#define ASM_ARGS_5	ASM_ARGS_4, "r" (_arg5)
#define ASM_ARGS_6	ASM_ARGS_5, "r" (_arg6)
#define ASM_ARGS_7	ASM_ARGS_6, "r" (_arg7)

/* Macros for converting sys-call wrapper args into sys call args
 * Note that this is just for sanity check as they are BOUND to be in same
 * order. The wrapper args by defn is to match sys-call args order.
 * e.g.
 * _syscall4( int, syscallname, vineet, rajesh, simon, mitesh)
 *  will cause the compiler to setup r0 = vineet, r1 = rajesh etc before
 *  invoking the sys-call wrapper. Thus args are already in ABI specified
 *  order for sys call
 */
#define LOAD_ARGS_0(__ret, arg)

/* Per ABI, r0 is 1st arg and return reg */
#define LOAD_ARGS_1(__ret, arg1) 					\
	__ret = (int) (arg1);

#define LOAD_ARGS_2(__ret, arg1, arg2)					\
	register int _arg2 __asm__ ("r1") = (int) (arg2);		\
	LOAD_ARGS_1 (__ret, arg1)

#define LOAD_ARGS_3(__ret, arg1, arg2, arg3)				\
	register int _arg3 __asm__ ("r2") = (int) (arg3);		\
	LOAD_ARGS_2 (__ret, arg1, arg2)

#define LOAD_ARGS_4(__ret, arg1, arg2, arg3, arg4)			\
	register int _arg4 __asm__ ("r3") = (int) (arg4);		\
	LOAD_ARGS_3 (__ret, arg1, arg2, arg3)

#define LOAD_ARGS_5(__ret, arg1, arg2, arg3, arg4, arg5)		\
	register int _arg5 __asm__ ("r4") = (int) (arg5);		\
	LOAD_ARGS_4 (__ret, arg1, arg2, arg3, arg4)

#define LOAD_ARGS_6(__ret,  arg1, arg2, arg3, arg4, arg5, arg6)		\
	register int _arg6 __asm__ ("r5") = (int) (arg6);		\
	LOAD_ARGS_5 (__ret, arg1, arg2, arg3, arg4, arg5)

#define LOAD_ARGS_7(__ret, arg1, arg2, arg3, arg4, arg5, arg6, arg7)	\
	register int _arg7 __asm__ ("r6") = (int) (arg7);		\
	LOAD_ARGS_6 (__ret, arg1, arg2, arg3, arg4, arg5, arg6)

#else /* ! __CONFIG_ARCH_ARC_A7__ */

/*-------------------------------------------------------------------------
  non ARC700
--------------------------------------------------------------------------*/

#define INTERNAL_SYSCALL_NCS(nm, err, nr_args, args...)			\
({									\
	register int _r0 __asm__ ("r0");				\
	LOAD_ARGS_##nr_args (args);					\
									\
	/* First, check if the AUX_HINT_REG is 0... ie, no interrupt	\
	 * is currently pending/being serviced				\
	 */								\
        __asm__ volatile (						\
		"1:     \n\t"						\
		"lr     r26, [%1] \n\t"					\
		"mov.f  0, r26 \n\t"					\
		"nop    \n\t"						\
		"bnz    1b \n\t"					\
									\
		/* FIXME: do we need to save the sp here? */		\
		"mov r0, %2 \n\t"   /* save the syscall type in r0 */	\
									\
		"sr %0, [%1] \n\t"  /* do the interrupt (syscall) */	\
		"nop \n\t" /* two nops to flush pipeline (hw issue) */	\
		"nop \n\t"						\
									\
	     : 								\
	     : "i" (SYSCALL_IRQ), "i" (AUX_IRQ_HINT),			\
	       "ir" (nm)) ASM_ARGS_##nr_args				\
	     : "r0","memory");						\
									\
	_r0;								\
})
					\
#define ASM_ARGS_0
#define ASM_ARGS_1	ASM_ARGS_0, "r" (_r1)
#define ASM_ARGS_2	ASM_ARGS_1, "r" (_r2)
#define ASM_ARGS_3	ASM_ARGS_2, "r" (_r3)
#define ASM_ARGS_4	ASM_ARGS_3, "r" (_r4)
#define ASM_ARGS_5	ASM_ARGS_4, "r" (_r5)
#define ASM_ARGS_6	ASM_ARGS_5, "r" (_r6)
#define ASM_ARGS_7	ASM_ARGS_6, "r" (_r7)

#define LOAD_ARGS_0()

#define LOAD_ARGS_1(r1)					\
	register int _r1 __asm__ ("r1") = (int) (r1);	\
	LOAD_ARGS_0 ()

#define LOAD_ARGS_2(r1, r2)				\
	register int _r2 __asm__ ("r2") = (int) (r2);	\
	LOAD_ARGS_1 (r1)

#define LOAD_ARGS_3(r1, r2, r3)				\
	register int _r3 __asm__ ("r3") = (int) (r3);	\
	LOAD_ARGS_2 (r1, r2)

#define LOAD_ARGS_4(r1, r2, r3, r4)			\
	register int _r4 __asm__ ("r4") = (int) (r4);	\
	LOAD_ARGS_3 (r1, r2, r3)

#define LOAD_ARGS_5(r1, r2, r3, r4, r5)			\
	register int _r5 __asm__ ("r5") = (int) (r5);	\
	LOAD_ARGS_4 (r1, r2, r3, r4)

#define LOAD_ARGS_6(r1, r2, r3, r4, r5, r6)		\
	register int _r6 __asm__ ("r6") = (int) (r6);	\
	LOAD_ARGS_5 (r1, r2, r3, r4, r5)

#define LOAD_ARGS_7(r1, r2, r3, r4, r5, r6, r7)		\
	register int _r7 __asm__ ("r7") = (int) (r7);	\
	LOAD_ARGS_6 (r1, r2, r3, r4, r5, r6)

#endif /* __CONFIG_ARCH_ARC_A7__ */

/*-------------------------------------------------------------------------
  Generic syscall helpers used by uClibc
--------------------------------------------------------------------------*/

#undef _syscall0
#define _syscall0(type,name) \
type name(void) \
{ \
return (type) (INLINE_SYSCALL(name, 0)); \
}

#undef _syscall1
#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
return (type) (INLINE_SYSCALL(name, 1, arg1)); \
}

#undef _syscall2
#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
return (type) (INLINE_SYSCALL(name, 2, arg1, arg2)); \
}

#undef _syscall3
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
return (type) (INLINE_SYSCALL(name, 3, arg1, arg2, arg3)); \
}

#undef _syscall4
#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
return (type) (INLINE_SYSCALL(name, 4, arg1, arg2, arg3, arg4)); \
}

#undef _syscall5
#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
return (type) (INLINE_SYSCALL(name, 5, arg1, arg2, arg3, arg4, arg5)); \
}

#undef _syscall6
#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5,type6,arg6) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5, type6 arg6) \
{ \
return (type) (INLINE_SYSCALL(name, 6, arg1, arg2, arg3, arg4, arg5, arg6)); \
}

#undef _syscall7
#define _syscall7(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5,type6,arg6,type7,arg7) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5, type6 arg6,type7 arg7) \
{ \
return (type) (INLINE_SYSCALL(name, 7, arg1, arg2, arg3, arg4, arg5, arg6, arg7)); \
}

#endif /* __ASSEMBLER__ */

#endif /* _BITS_SYSCALLS_H */
