/******************************************************************************
 * Copyright Synopsys, Inc. (www.synopsys.com) Oct 01, 2004
 * 
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 *****************************************************************************/
/* Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org> */
/* Copyright (C) 2007 Synopsys, Inc. (www.synopsys.com) */

/*
 * Various assmbly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 */

#include "elf.h"

/*
 * Define this if the system uses RELOCA.
 */
#define ELF_USES_RELOCA

/*
 * Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.
 */
#define GET_ARGV(ARGVP, ARGS) ARGVP = ((unsigned long*) ARGS + 1)

#define BITS(word,s,e)          (((word) << (31 - e)) >> (s + (31 -e)))
#define GOTBASE_IN_PLT0	 20	
/*
 * Initialization sequence for a GOT.The GOT Base that we always
 * receive is the DT_PLTGOT value for everybody other than the 
 * dynamic linker . Hence the check for the program interpreter in the
 * INIT_GOT macro. (The value of the GOT Base is at DT_PLTGOT (which points to the
 * plt0 entry) + 20 )
 */
#define INIT_GOT(GOT_BASE,MODULE)				   \
do {								   \
  if(MODULE->libtype != program_interpreter)		   \
	GOT_BASE = (unsigned long *)(*(unsigned long *)((char *)(GOT_BASE) + GOTBASE_IN_PLT0) + (unsigned long)MODULE->loadaddr); \
  GOT_BASE[2] = (unsigned long) _dl_linux_resolve;		   \
  GOT_BASE[1] = (unsigned long) MODULE;				   \
} while(0)

/*
 * Dynamic loader bootstrapping:
 * Since we don't modify text at runtime, these can only be data relos
 * (so r to assume that they are word aligned.
 * @RELP is the relo entry being processed
 * @REL is the pointer to the address we are relocating.
 * @SYMBOL is the symbol involved in the relocation
 * @LOAD is the load address.
 */

#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD,SYMTAB)	\
do {															\
	int type = ELF32_R_TYPE((RELP)->r_info);					\
	if (likely(type == R_ARC_RELATIVE))							\
		*REL += (unsigned long) LOAD;							\
	else if ((type == R_ARC_GLOB_DAT) || type == R_ARC_JMP_SLOT)\
		*REL = SYMBOL;											\
	else														\
		_dl_exit(1);											\
}while(0)


/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  This routine has to exit the current function, then 
 * call the _dl_elf_main function.
 */
#define START()	return _dl_elf_main 

/* Here we define the magic numbers that this dynamic loader should accept */

#define MAGIC1 EM_ARCOMPACT
#undef  MAGIC2
/* Used for error messages */
#define ELF_TARGET "ARC"

struct elf_resolve;
extern unsigned long _dl_linux_resolver(struct elf_resolve * tpnt,
							unsigned int plt_pc);

extern unsigned __udivmodsi4 (unsigned, unsigned)
  __attribute ((visibility("hidden")));

#define do_rem(result, n, base)  \
  ((result) \
   = (__builtin_constant_p (base) \
      ? (n) % (unsigned) (base) \
      : ({ \
	   register unsigned r1 __asm__ ("r1") = (base); \
 \
	   __asm("bl.d @__udivmodsi4` mov r0,%1" : "=r" (r1) \
	         : "r" (n), "r" (r1) \
	         : "r0", "r2", "r3", "r4", "lp_count", "blink", "cc"); \
	   r1; })))

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_ARC_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_ARC_COPY) * ELF_RTYPE_CLASS_COPY))

/* Get the runtime address of GOT[0]
 */
static inline Elf32_Addr elf_machine_dynamic (void) attribute_unused;
static inline Elf32_Addr
elf_machine_dynamic (void)
{
  Elf32_Addr dyn;

  __asm__("ld %0,[pcl,_DYNAMIC@gotpc]\n\t" : "=r" (dyn));
  return dyn;

/* Another way would have been to simply return GP, which due to some
 * PIC reference would be automatically setup by gcc in caller
	register Elf32_Addr *got __asm__ ("gp"); return *got;
 */
}

/* Return the run-time load address of the shared object.  */
static inline Elf32_Addr elf_machine_load_address (void) attribute_unused;
static inline Elf32_Addr
elf_machine_load_address (void)
{
    /* To find the loadaddr we subtract the runtime addr of any symbol
     * say _dl_start from it's build-time addr.
     */
	Elf32_Addr addr, tmp;
	__asm__ (
        "ld  %1, [pcl, _dl_start@gotpc] ;build addr of _dl_start   \n"
        "add %0, pcl, _dl_start-.+(.&2) ;runtime addr of _dl_start \n"
        "sub %0, %0, %1                 ;delta                     \n"
        : "+r" (addr), "+r"(tmp)
    );
	return addr;
}

static inline void
elf_machine_relative (Elf32_Addr load_off, const Elf32_Addr rel_addr,
		      Elf32_Word relative_count)
{
	 Elf32_Rel * rpnt = (void *) rel_addr;
	--rpnt;
	do {
		Elf32_Addr *const reloc_addr = (void *) (load_off + (++rpnt)->r_offset);

		*reloc_addr += load_off;
	} while (--relative_count);
}
