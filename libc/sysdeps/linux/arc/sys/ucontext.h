/* System V/ARC ABI compliant context switching support.  */

#ifndef _SYS_UCONTEXT_H
#define _SYS_UCONTEXT_H	1

#include <features.h>
#include <signal.h>
#include <sys/procfs.h>

typedef int greg_t;

/* Number of general registers.  */
#define NGREG	26

/* Container for all general registers.  */
typedef elf_gregset_t gregset_t;

/* Number of each register is the `gregset_t' array.  */
enum
{
  R0 = 0,
#define R0	R0
  R1,
#define R1	R1
  R2,
#define R2	R2
  R3,
#define R3	R3
  R4,
#define R4	R4
  R5,
#define R5	R5
  R6,
#define R6	R6
  R7,
#define R7	R7
  R8,
#define R8	R8
  R9,
#define R9	R9
  R10,
#define R10	R10
  R11,
#define R11	R11
  R12,
#define R12	R12
  R13,
#define R13	R13
  R14,
#define R14	R14
  R15,
#define R15	R15
  R16,
#define R16	R16
  R17,
#define R17	R17
  R18,
#define R18	R18
  R19,
#define R19	R19
  R20,
#define R20	R20
  R21,
#define R21	R21
  R22,
#define R22	R22
  R23,
#define R23	R23
  R24,
#define R24	R24
  R25,
#define R25	R25
  R26,
#define R26	R26
};

/* Structure to describe FPU registers.  */
typedef elf_fpregset_t	fpregset_t;

/* Context to describe whole processor state.  */
typedef struct
  {
    gregset_t gregs;
    fpregset_t fpregs;
  } mcontext_t;

/* Userlevel context.  */
typedef struct ucontext
  {
    unsigned long int uc_flags;
    struct ucontext *uc_link;
    __sigset_t uc_sigmask;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    long int uc_filler[5];
  } ucontext_t;

#endif /* sys/ucontext.h */
