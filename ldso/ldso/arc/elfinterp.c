/******************************************************************************
 * Copyright Synopsys, Inc. (www.synopsys.com) 2012
 *
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 *****************************************************************************/

/* vi: set sw=4 ts=4: */
/* i386 ELF shared library loader suppport
 *
 * Copyright (c) 1994-2000 Eric Youngdale, Peter MacDonald, 
 *				David Engel, Hongjiu Lu and Mitch D'Souza
 * Copyright (C) 2001-2002, Erik Andersen
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the above contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * vineetg: Jan 7th 2011
 *  -Major refactoring of code mainly to speedup the relo processing
 *  -Removed function-call overhead overhead per relo entry processed
 *   by converting the function ptr for lazy/now blocks into inline funcs
*/

#include "ldso.h"

extern int _dl_linux_resolve(void);

unsigned long _dl_linux_resolver(struct elf_resolve *tpnt, unsigned int plt_pc)
{
	ELF_RELOC *this_reloc, *rel_base;
	char *strtab, *symname, *new_addr;
	Elf32_Sym *symtab;
	int symtab_index;
	char **got_addr;
	unsigned long plt_base;
	int plt_idx;

	/* start of .rela.plt */
	rel_base = (ELF_RELOC *)(tpnt->dynamic_info[DT_JMPREL]);

	/* starts of .plt (addr of PLT0) */
	plt_base = tpnt->dynamic_info[DT_PLTGOT];

	/* compute the idx of the yet-unresolved PLT entry in .plt
       Same idx will be used to find the relo entry in .rela.plt
	*/
	plt_idx = ((plt_pc - plt_base) / 0xc  - 2); // ignoring 2 dummy PLTs

	this_reloc = rel_base + plt_idx;

	symtab_index = ELF32_R_SYM(this_reloc->r_info);
	symtab = (Elf32_Sym *)(intptr_t) (tpnt->dynamic_info[DT_SYMTAB]);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB]);
	symname= strtab + symtab[symtab_index].st_name;

	/* relo-offset to fixup, shd be a .got entry */
	got_addr = (char **)((unsigned long) this_reloc->r_offset +
			             (unsigned long) tpnt->loadaddr);

	/* Get the address of the GOT entry */
	new_addr = _dl_find_hash(symname, tpnt->symbol_scope, tpnt,
                            ELF_RTYPE_CLASS_PLT);

	if (unlikely(!new_addr)) {
		new_addr = _dl_find_hash(symname, NULL, NULL, ELF_RTYPE_CLASS_PLT);
		if (new_addr) {
			return (unsigned long) new_addr;
		}
		_dl_dprintf(2, "%s: can't resolve symbol '%s'\n", _dl_progname, symname);
		_dl_exit(1);
	}


#if !defined (__SUPPORT_LD_DEBUG__)
	/* Update the .got entry with the runtime address of symbol */
	*got_addr = new_addr;
#else
	if (_dl_debug_bindings)
	{
		_dl_dprintf(_dl_debug_file, "\nresolve function: %s", symname);
		if(_dl_debug_detail) _dl_dprintf(_dl_debug_file,
			"\n\tpatched %x ==> %x @ %x\n", *got_addr, new_addr, got_addr);
	}
    if (!_dl_debug_nofixups)
    {
        *got_addr = new_addr;
    }
#endif
    /* return the new addres, where the asm trampoline will jump to
     *  after re-setting up the orig args
     */
	return (unsigned long) new_addr;
}

static int
_dl_do_lazy_reloc (struct elf_resolve *tpnt, ELF_RELOC *rpnt);

static int
_dl_do_reloc (struct elf_resolve *tpnt,struct dyn_elf *scope,
	      ELF_RELOC *rpnt, Elf32_Sym *symtab, char *strtab);

#define ___DO_LAZY  1
#define ___DO_NOW   2

static int __attribute__((always_inline))
_dl_parse(struct elf_resolve *tpnt,
	  unsigned long rel_addr, unsigned long rel_size, int type)
{
	unsigned int i;
	char *strtab;
	Elf32_Sym *symtab;
	ELF_RELOC *rpnt;
	int symtab_index;
	int res = 0;
    struct dyn_elf *scope;

    if (type == ___DO_NOW)
        scope = tpnt->symbol_scope;
    else
        scope = NULL;

	/* Now parse the relocation information */
	rpnt = (ELF_RELOC *)(intptr_t) (rel_addr);
	rel_size = rel_size / sizeof(ELF_RELOC);

	symtab = (Elf32_Sym *)(intptr_t) (tpnt->dynamic_info[DT_SYMTAB]);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB]);

	for (i = 0; i < rel_size; i++, rpnt++) {

		symtab_index = ELF32_R_SYM(rpnt->r_info);

		/* When the dynamic linker bootstrapped itself, it resolved some symbols.
		   Make sure we do not do them again */
		if (tpnt->libtype == program_interpreter) {
		    if (!symtab_index ||
					_dl_symbol(strtab + symtab[symtab_index].st_name))
			    continue;
        }
#if defined (__SUPPORT_LD_DEBUG__)
		debug_sym(symtab,strtab,symtab_index);
		debug_reloc(symtab,strtab,rpnt);
#endif

        /* By contant propgation the 'if' case is subsumed as function
         *  call inlined
         */
        if (type == ___DO_LAZY)
		    res = _dl_do_lazy_reloc(tpnt, rpnt);
        else
		    res = _dl_do_reloc(tpnt, scope, rpnt, symtab, strtab);

		if (res==0) continue;
		else break;
	}

	if (unlikely(res <0))
	{
		int reloc_type = ELF32_R_TYPE(rpnt->r_info);
#if defined (__SUPPORT_LD_DEBUG__)
		_dl_dprintf(2, "can't handle reloc type %s\n ", _dl_reltypes(reloc_type));
#else
		_dl_dprintf(2, "can't handle reloc type %x\n", reloc_type);
#endif
		_dl_exit(-res);
	}
	return res;
}


static int
_dl_do_reloc (struct elf_resolve *tpnt,struct dyn_elf *scope,
	      ELF_RELOC *rpnt, Elf32_Sym *symtab, char *strtab)
{
	int reloc_type;
	int symtab_index;
	char *symname;
	unsigned long *reloc_addr;
	unsigned long symbol_addr;
    unsigned long addend;
#if defined (__SUPPORT_LD_DEBUG__)
	unsigned long old_val;
#endif

	reloc_addr   = (unsigned long *)(tpnt->loadaddr + (unsigned long) rpnt->r_offset);
	reloc_type   = ELF32_R_TYPE(rpnt->r_info);
	symtab_index = ELF32_R_SYM(rpnt->r_info);
	symbol_addr  = 0;

#if defined (__SUPPORT_LD_DEBUG__)
    old_val = *reloc_addr;
#endif

	if (symtab_index) {
	    symname      = strtab + symtab[symtab_index].st_name;
		symbol_addr = (unsigned long) _dl_find_hash(symname, scope,
							    tpnt, elf_machine_type_class(reloc_type));

		/*
		 * We want to allow undefined references to weak symbols - this might
		 * have been intentional.  We should not be linking local symbols
		 * here, so all bases should be covered.
		 */

		if (unlikely(!symbol_addr &&
				ELF32_ST_BIND(symtab[symtab_index].st_info) == STB_GLOBAL)) {
#if defined (__SUPPORT_LD_DEBUG__)
			_dl_dprintf(2, "\tglobal symbol '%s' already defined in '%s'\n",
					symname, tpnt->libname);
#endif
			return 0;
		}
	}
    else if (reloc_type == R_ARC_RELATIVE )
    {
        *reloc_addr += (unsigned long) tpnt->loadaddr;
        goto log_entry;
    }

	switch (reloc_type) {
		case R_ARC_32:
			*reloc_addr += symbol_addr + rpnt->r_addend;
			break;
		case R_ARC_PC32:
			*reloc_addr += symbol_addr + rpnt->r_addend
								- (unsigned long) reloc_addr;
			break;
		case R_ARC_GLOB_DAT:
		case R_ARC_JMP_SLOT:
			*reloc_addr = symbol_addr;
			break;
		case R_ARC_COPY:
			_dl_memcpy((void *) reloc_addr,
				         (void *) symbol_addr, symtab[symtab_index].st_size);
			break;
		default:
			_dl_dprintf(1, "unknown Relocation type\n");
			return -1; /*call _dl_exit(1) */
	}

log_entry:
#if defined (__SUPPORT_LD_DEBUG__)
	if(_dl_debug_detail)
		_dl_dprintf(_dl_debug_file,"\tpatched: %x ==> %x @ %x: addend %x ",
                    old_val, *reloc_addr, reloc_addr, rpnt->r_addend);
#endif

	return 0;
}

static int
_dl_do_lazy_reloc (struct elf_resolve *tpnt, ELF_RELOC *rpnt)
{
	int reloc_type;
	unsigned long *reloc_addr;
#if defined (__SUPPORT_LD_DEBUG__)
	unsigned long old_val;
#endif

	reloc_addr = (unsigned long *)(intptr_t) (tpnt->loadaddr + (unsigned long) rpnt->r_offset);
	reloc_type = ELF32_R_TYPE(rpnt->r_info);

#if defined (__SUPPORT_LD_DEBUG__)
	old_val = *reloc_addr;
#endif
		switch (reloc_type) {
			case R_ARC_JMP_SLOT:
				*reloc_addr += (unsigned long) tpnt->loadaddr;
				break;
			default:
				return -1; /*call _dl_exit(1) */
		}
#if defined (__SUPPORT_LD_DEBUG__)
	if(_dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\tpatched: %x ==> %x @ %x", old_val, *reloc_addr, reloc_addr);
#endif
	return 0;

}

void _dl_parse_lazy_relocation_information(struct dyn_elf *rpnt,
	unsigned long rel_addr, unsigned long rel_size)
{
    /* This func is called for processing .rela.plt of loaded module(s)
     * The relo entries handled are JMP_SLOT type for fixing up .got slots for
     * external function calls.
     * This function doesn't resolve the slots: that is done lazily at runtime.
     * The build linker (at least thats what happens for ARC) had pre-init the
     * .got slots to point to PLT0. All that is done here is to fix them up to
     * point to load value of PLT0 (as opposed to the build value).
     * On ARC, the loadaddr of dyn exec is zero, thus elfaddr == loadaddr
     * Thus there is no point in adding "0" to values and un-necessarily stir
     * up the caches and TLB.
     * For lsdo processing busybox binary, this skips over 380 relo entries
     */
    if (rpnt->dyn->loadaddr != 0)
		_dl_parse(rpnt->dyn, rel_addr, rel_size, ___DO_LAZY);
}

int _dl_parse_relocation_information(struct dyn_elf  *rpnt,
	unsigned long rel_addr, unsigned long rel_size)
{
	return _dl_parse(rpnt->dyn, rel_addr, rel_size, ___DO_NOW);
}

