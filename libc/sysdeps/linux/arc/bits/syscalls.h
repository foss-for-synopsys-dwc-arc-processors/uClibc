/* Syscall Macro Generator for ARC
 *
 * - Originally written by Codeito, who in turn borrowed it from ARM/???
 * - vineet.gupta@viragelogic.com: Dec 1st 2009 : Rewrote for optimizations
 *     * NOP(s) not needed after TRAP insn
 *     * Go rid of superfluous reg setups as they are already in syscall
 *       ABI comatible order (syscall ABI == function call ABI)
 *     * errno handling moved out-ofline to avoid unconditional push/pop
 *
 */
#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
#error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#ifndef __ASSEMBLER__

#include <errno.h>

/* call for errno-setting seperated off to avoid push/pop unconditionally */

#ifdef IS_IN_rtld
/* ldso doesn't have real errno */
#define ERRNO_ERRANDS(_sys_result)
#else /* !IS_IN_rtld */
extern int __syscall_error (int);
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

#define INLINE_SYSCALL_NOERR(name, nr, args...)				   \
  ({ unsigned int _inline_sys_result = INTERNAL_SYSCALL (name, , nr, args);\
     (int) _inline_sys_result; })

/*-------------------------------------------------------------------------
 * Mechanics of Trap - specific to ARC700
 *
 * Note the memory clobber is not strictly needed for intended semantics of
 * the inline asm. However some of the cases, such as old-style 6 arg mmap
 * gcc was generating code for inline syscall ahead of buffer packing needed
 * for syscall itself.
 *-------------------------------------------------------------------------*/

#if defined(__A7__)
#define ARC_TRAP_INSN	"trap0          \n\t"
#elif defined(__EM__)
#define ARC_TRAP_INSN	"trap_s 0        \n\t"
#endif

#define INTERNAL_SYSCALL_NCS(nm, err, nr_args, args...)	\
({							\
	/* Per ABI, r0 is 1st arg and return reg */	\
	register int __ret __asm__("r0");		\
	register int _sys_num __asm__("r8") = (nm);	\
	LOAD_ARGS_##nr_args (__ret, args)		\
							\
        __asm__ volatile (				\
		ARC_TRAP_INSN				\
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

/*
 * Note that the use of _tmpX might look superflous, however it is needed
 * to ensure that register variables are not clobbered if arg happens to be
 * a function call itself. e.g. sched_setaffinity() calling getpid() for arg2
 */
#define LOAD_ARGS_2(__ret, arg1, arg2)					\
	int _tmp2 = (int) (arg2);					\
	LOAD_ARGS_1 (__ret, arg1)					\
	register int _arg2 __asm__ ("r1") = _tmp2;

#define LOAD_ARGS_3(__ret, arg1, arg2, arg3)				\
	int _tmp3 = (int) (arg3);					\
	LOAD_ARGS_2 (__ret, arg1, arg2)					\
	register int _arg3 __asm__ ("r2") = _tmp3;

#define LOAD_ARGS_4(__ret, arg1, arg2, arg3, arg4)			\
	int _tmp4 = (int) (arg4);					\
	LOAD_ARGS_3 (__ret, arg1, arg2, arg3)				\
	register int _arg4 __asm__ ("r3") = _tmp4;

#define LOAD_ARGS_5(__ret, arg1, arg2, arg3, arg4, arg5)		\
	int _tmp5 = (int) (arg5);					\
	LOAD_ARGS_4 (__ret, arg1, arg2, arg3, arg4)			\
	register int _arg5 __asm__ ("r4") = _tmp5;

#define LOAD_ARGS_6(__ret,  arg1, arg2, arg3, arg4, arg5, arg6)		\
	int _tmp6 = (int) (arg6);					\
	LOAD_ARGS_5 (__ret, arg1, arg2, arg3, arg4, arg5)		\
	register int _arg6 __asm__ ("r5") = _tmp6;

#define LOAD_ARGS_7(__ret, arg1, arg2, arg3, arg4, arg5, arg6, arg7)	\
	int _tmp7 = (int) (arg7);					\
	LOAD_ARGS_6 (__ret, arg1, arg2, arg3, arg4, arg5, arg6)		\
	register int _arg7 __asm__ ("r6") = _tmp7;

#else

#if defined(__A7__)
#define ARC_TRAP_INSN	trap0
#elif defined(__EM__)
#define ARC_TRAP_INSN	trap_s 0
#endif

#endif /* __ASSEMBLER__ */

#endif /* _BITS_SYSCALLS_H */
