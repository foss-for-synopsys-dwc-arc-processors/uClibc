/* When we enter this piece of code, the program stack looks like this:
        argc            argument counter (integer)
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
	NULL
        env[0...N]      environment variables (pointers)
        NULL

   For uClinux it looks like this:

        argc            argument counter (integer)
        argv            char *argv[]
        envp            char *envp[]
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
	NULL
        env[0...N]      environment variables (pointers)
        NULL
	
   When we are done here, we want
	a1=argc
	a2=argv[0]
	a3=argv[argc+1]
*/


#define __ASSEMBLY__
#include <features.h>
#include <sys/syscall.h>
.text
	.global __start
	.type	__start,@function

#ifndef __UCLIBC_CTOR_DTOR__
	.weak	_init
	.weak	_fini
#endif
/* Stick in a dummy reference to main(), so that if an application
 * is linking when the main() function is in a static library (.a)
 * we can be sure that main() actually gets linked in */
	.type	main,@function
	.type	_main,@function

.text

__start:	
.global __start
	/* clear the frame pointer */
 	mov	fp, 0
	ld_s	r1, [sp]	 ; argc
#ifdef __ARCH_USE_MMU__
	mov_s	r5, r0; rltd_fini
	add_s	r2, sp, 4	; argv
#else
	mov_s	r5, 0
	ld_s	r2, [sp, 4]	;argv
#endif

	mov_s	r0, main
	mov_s	r3, _init
	mov	r4, _fini
;;; 	mov	r3, 0x0
;;; 	mov	r4, 0x0

	and	sp, sp, -8
	mov	r6, sp

	/* __uClibc_main (main, argc, argv, init, fini, rtld_fini, stack_end) */
	bl.nd	__uClibc_main
	/* Should never get here....  */
	b	_exit_halt

	.global	_exit_halt
	.type	_exit_halt, @function

_exit_halt:
	flag	0x01
	nop
	nop
	nop
	b	_exit_halt
	nop

	.end
	
