2012-05-22  Vineet Gupta  <vgupta@synopsys.com>

	* libc/sysdeps/linux/arc/bits/syscalls.h: Provide syscall macros
	INTERNAL_SYSCALL{,_DECL,_ERRNO,_ERROR_P} and
	INLINE_SYSCALL{,_NCS}
	* libc/sysdeps/linux/arc/syscall.c: Uses INLINE_SYSCALL_NCS

2012-04-30  Joern Rennecke  <joern.rennecke@embecosm.com>

	* libc/sysdeps/linux/arc/bits/endian.h: Check __BIG_ENDIAN__.

2012-04-11  Vineet Gupta  <vgupta@synopsys.com>

	* arc_config (LDSO_STATS_SUPPORT): Enable.
	* extra/Configs/Config.in (LDSO_STATS_SUPPORT): New.
	* ldso/include/dl-syscall.h: Make _dl_gettimeofday also
	available for __LDSO_STATS_SUPPORT__.
	* ldso/ldso/ldso.c (_dl_debug_statas) [__LDSO_STATS_SUPPORT__]:
	New variable.
	(_dl_get_ready_to_run) [__LDSO_STATS_SUPPORT__]: Print time to load.

	* ldso/ldso/arc/elfinterp.c [0] (_dl_do_copy): Delete.
	[0] (_dl_parse_copy_information): Likewise.
	* ldso/ldso/arc/dl-sysdep.h: Remove commented out code.
	(arc_modulus): Delete.
	[0] (COPY_HASH, ELF_RESOLVE_TDEP): Delete.
	* ldso/ldso/arc/resolve.S: Remove trailing whitespace.
	Remove factually false comments.

	* ldso/ldso/arc/dl-startup.h (LEAPCREL, LDPCREL): Delete.
	(_start): Avoid multiline string literal.  Optimize.
	(NO_EARLY_SEND_STDERR): Define.
	* ldso/ldso/arc/dl-sysdep.h (arc_read_me, arc_write_me): Delete.
	(extract_arc_pc_rel_25, insert_arc_pc_rel_25): Delete.
	(PERFORM_BOOTSTRAP_RELOC): Remove support for non-data relocs.
	(elf_machine_dynamic, elf_machine_load_address): Remove use of gp.

	* ldso/ldso/ldso.c (_dl_get_ready_to_run [__A7__]): Remove ARC LOCAL
	hack.

	Hacks to improve inlining:
	* ldso/ldso/arc/elfinterp.c: Add function prototypes.
	(_dl_linux_resolver): Rename second parameter.  Revamp types used
	to calculate pointer to used relocation.  Use "unlikely".
	Reorder some code.
	(_dl_parse): Change argument list.  Changed all callers.
	(_dl_do_lazy_reloc): Likewise.
	* ldso/ldso/arc/dl-sysdep.h (_dl_linux_resolver): Rename second
	parameter.

	* ldso/ldso/arc/elfinterp.c (_dl_do_reloc): Assume relocation
	addresses that need to be operated on are properly aligned.
	(_dl_parse_lazy_relocation_information): Don't call dl_parse
	if the loadaddress is zero.

	* ldso/ldso/arc/resolve.S (SAVE_CALLER_SAVED): Don't save
	callee-saved registers, nor registers that have already
	been clobbered by the plt.
	(RESTORE_CALLER_SAVED): Rename to ..
	(RESTORE_CALLER_SAVED_BUT_R0): .. this.  Don't restore registers
	that have now been excluded from SAVE_CALLER_SAVED, not r0.
	(_dl_linux_resolve): Take advantage of above changes to simplify
	handling of resolved address.

	* ldso/ldso/Makefile.in [ifeq ($(TARGET_ARCH),arc)] (CFLAGS-ldso):
	* Rules.mak [ifeq ($(TARGET_ARCH),arc)] (OPTIMIZATION):
	Don't add -fno-tree-loop-optimize / -fno-tree-dominator-opts /
	-fno-strength-reduce.
	Don't add -fomit-frame-pointer.
	[ifeq ($(TARGET_ARCH),arc)] (CPU_CFLAGS-$(CONFIG_ARCH_ARC_A4):
	Don't set.
	[ifeq ($(TARGET_ARCH),arc)] (CPU_CFLAGS-$(CONFIG_ARCH_ARC_A5):
	Likewise.
	[ifeq ($(TARGET_ARCH),arc)] (CPU_CFLAGS-$(CONFIG_ARCH_ARC_A7):
	Remove -fno-peephole.
	* arc_config (LDSO_CACHE_SUPPORT, LDSO_BASE_FILENAME): Don't set.

2012-03-29  Jeremy Bennett  <jeremy.bennett@embecosm.com>

	* arc_config <CROSS_COMPILER_PREFIX>: make generic
	%CROSS_COMPILER_PREFIX% to allow full substitution by scripts.

2012-03-01  Jeremy Bennett  <jeremy.bennett@embecosm.com>

	* libc/stdlib/_atexit.c <__dso_handle>: Add hidden visibility
	attribute to extern definition.
	* ldso/ldso/Makefile.in: Do not use --no-undefined when linking
	with debug enabled.

2012-01-07  Jeremy Bennett  <jeremy.bennett@embecosm.com>

	* arc_config <KERNEL_HEADERS>: make generic %KERNEL_HEADERS% to
	allow full substitution by scripts.
	<RUNTIME_PREFIX>: make generic %RUNTIME_PREFIX% to allow full
	substitution by scripts.
	<DEVEL_PREFIX>: make generic %DEVEL_PREFIX% to allow full
	substitution by scripts.
	<UCLIBC_EXTRA_FLAGS>: Remove -isystem, since the correct
	installation of Linux headers makes this obsolete.
	* libc/sysdeps/linux/arc/bits/syscalls.h:
	* libc/sysdeps/linux/arc/vfork.S:
	Comment out inclusion of non-existant header, asm/system.h
	* libc/sysdeps/linux/arc/sys/user.h: Enable definition of
	structures as with other embedded architectures. Alternative
	include referenced non-existant Linux header.

2011-12-02  Joern Rennecke  <joern.rennecke@embecosm.com>

	From Alexey Brodkin:
	* libc/sysdeps/linux/arc/syscall.c (syscall): Fix passed argument list.

	From Alexey Brodkin:
	* libc/sysdeps/linux/arc/bits/kernel_stat.h (struct kernel_stat64):
	Change type of st_blocks to unsigned long long.

2011-06-28     Vineet Gupta  <vgupta@synopsys.com>
    * libc/sysdeps/linux/arc/mmap.c (mmap): Tweaked the prev fix
    Need to make sure that operand, not operation, is sign safe.

2011-06-21  Brendan Kehoe  <brendan@zen.org>

	* libc/sysdeps/linux/arc/mmap.c (mmap): Fix OFFSET shift in
	return to use __u_long instead of default signedness of off_t.

2010-10-19  Brendan Kehoe  <brendan@zen.org>

	* libpthread/linuxthreads/sysdeps/arc/pt-machine.h (PT_EI):
	Change to __extern_always_inline to match the linuxthreads.old
	version just so they're in sync.

2010-09-28  Brendan Kehoe  <brendan@zen.org>

	* Delete .config and .config.old; the .config script should
	be created by the build script.

2010-09-27  Brendan Kehoe  <brendan@zen.org>

	* arc_config (UCLIBC_PREGENERATED_LOCALE_DATA,
	UCLIBC_DOWNLOAD_PREGENERATED_LOCALE_DATA): Put these back to 'n'
	to follow the suggestion of the 0.9.30/0.9.30.3 defaults.

2010-09-24  Brendan Kehoe  <brendan@zen.org>

	Update to 0.9.30.3.
	* ldso/ldso/arc/dl-startup.h: Change asm() to __asm__().
	* ldso/ldso/ldso.c: Likewise.
	* libc/sysdeps/linux/arc/bits/syscalls.h: Likewise.
	* libc/sysdeps/linux/arc/brk.c (brk): Likewise.
	* extra/locale/Makefile.in (LOCALE_DATA_FILENAME): Revert this
	back to the prev version's uClibc-locale-030818.tgz since
	the 081111 version is not actually available from uclibc.org.
	* arc_config: New file.
	* Rules.mak: Add ARC-specific optimizations.
	* extra/Configs/Config.arc: New file.
	* extra/Configs/Config.in (TARGET_arc): New config.
	[TARGET_arc]: Source extra/Configs/Config.arc.
	* include/elf.h (EM_ARCOMPACT, R_ARC_*): Define.
	* ldso/ldso/arc: New dir.
	* ldso/include/dl-hash.h [__A7__]: Add __dl_.
	* ldso/ldso/dl-hash.c (_dl_elf_hash): Do higher-performance
	version back-translated from ARC assembly.
	* ldso/ldso/ldso.c (_dl_get_ready_to_run) [__A7__]: Add asm to
	load r26 into LPNT.
	* src/libc/string/arc: New dir.
	* src/libc/sysdeps/linux/arc: New dir.
	* src/libpthread/linuxthreads/sysdeps/arc: New dir.
	* src/libpthread/linuxthreads/sysdeps/unix/sysv/linux/arc: New dir.
	* src/libpthread/linuxthreads.old/sysdeps/arc: New dir.
	* src/utils/ldd.c (MATCH_MACHINE, ELFCLASSM) [__A7__]: Define.

2010-07-19  Brendan Kehoe  <brendan@zen.org>

	* libc/sysdeps/linux/arc/bits/kernel_types.h (__kernel_old_dev_t):
	Add definition from 0.9.29.

2010-06-19  Brendan Kehoe  <brendan@zen.org>

	* libpthread/linuxthreads.old/sysdeps/arc/pt-machine.h (PT_EI):
	Change to use __extern_always_inline to make sure we don't get
	multiple definitions of testandset.  Do this instead of
	__GNUC_MINOR__ from .29.

2010-05-21  Brendan Kehoe  <brendan@zen.org>

	* libc/sysdeps/linux/arc/sigrestorer.S (__ASSEMBLY__, _ASM):
	Define so things like ptrace.h can end up being included
	without causing an error.

2010-05-14  Brendan Kehoe  <brendan@zen.org>

	* extra/scripts/unifdef.c (parseline): Rename from getline.
	(process): Change call.
	* cf http://linux.derkeiler.com/Mailing-Lists/Kernel/2009-03/msg04460.html

	* libc/sysdeps/linux/arc/bits/syscalls.h: Update from .29
	changes.
	* ldso/ldso/arc/dl-syscalls.h: Likewise.

2009-01-05  Irfan Rafiq  <irfan.rafiq@arc.com>

	* Merged ARC changes for uClibc v0.9.29 to v0.9.30. Modified files are


	* ldso/ldso/powerpc/dl-sysdep.h
	* ldso/ldso/ldso.c
	* ldso/ldso/dl-hash.c
	* ldso/ldso/arc/resolve.S
	* ldso/ldso/arc/dl-syscalls.h
	* ldso/ldso/arc/dl-startup.h
	* ldso/ldso/arc/elfinterp.c
	* ldso/ldso/arc/dl-debug.h
	* ldso/ldso/arc/dl-sysdep.h
	* ldso/include/dl-hash.h
	* libc/string/arc/memcmp.S
	* libc/string/arc/newlib
	* libc/string/arc/newlib/memcmp.S
	* libc/string/arc/newlib/configure
	* libc/string/arc/newlib/Makefile.in
	* libc/string/arc/newlib/configure.in
	* libc/string/arc/newlib/strcpy-600.S
	* libc/string/arc/newlib/strchr-a4.c
	* libc/string/arc/newlib/strcpy-700.S
	* libc/string/arc/newlib/strlen-a4.c
	* libc/string/arc/newlib/memcpy.S
	* libc/string/arc/newlib/strcmp-a4.c
	* libc/string/arc/newlib/memset.S
	* libc/string/arc/newlib/memcpy-700.S
	* libc/string/arc/newlib/setjmp.S
	* libc/string/arc/newlib/strcpy.c
	* libc/string/arc/newlib/strchr.S
	* libc/string/arc/newlib/asm.h
	* libc/string/arc/newlib/strlen.S
	* libc/string/arc/newlib/strcmp.S
	* libc/string/arc/newlib/Makefile.am
	* libc/string/arc/newlib/strchr-700.S
	* libc/string/arc/newlib/strlen-600.S
	* libc/string/arc/newlib/memset-a4.c
	* libc/string/arc/newlib/aclocal.m4
	* libc/string/arc/uclibc-asm.h
	* libc/string/arc/strcpy.S
	* libc/string/arc/strchr.S
	* libc/string/arc/strlen.S
	* libc/string/arc/strcmp.S
	* libc/string/arc/memcpy.S
	* libc/string/arc/memset.S
	* libc/string/arc/Makefile
	* libc/sysdeps/linux/common/sched_setaffinity.c
	* libc/sysdeps/linux/arc/vfork.S
	* libc/sysdeps/linux/arc/Makefile.arch
	* libc/sysdeps/linux/arc/syscall.c
	* libc/sysdeps/linux/arc/brk.c
	* libc/sysdeps/linux/arc/clone.S
	* libc/sysdeps/linux/arc/bits
	* libc/sysdeps/linux/arc/bits/kernel_stat.h
	* libc/sysdeps/linux/arc/bits/uClibc_arch_features.h
	* libc/sysdeps/linux/arc/bits/uClibc_page.h
	* libc/sysdeps/linux/arc/bits/syscalls.h
	* libc/sysdeps/linux/arc/bits/endian.h
	* libc/sysdeps/linux/arc/bits/stackinfo.h
	* libc/sysdeps/linux/arc/bits/setjmp.h
	* libc/sysdeps/linux/arc/bits/statfs.h
	* libc/sysdeps/linux/arc/bits/kernel_types.h
	* libc/sysdeps/linux/arc/bits/wordsize.h
	* libc/sysdeps/linux/arc/bits/fcntl.h
	* libc/sysdeps/linux/arc/bits/mman.h
	* libc/sysdeps/linux/arc/crti.S
	* libc/sysdeps/linux/arc/setjmp.S
	* libc/sysdeps/linux/arc/___longjmp.S
	* libc/sysdeps/linux/arc/__longjmp.S
	* libc/sysdeps/linux/arc/crtn.S
	* libc/sysdeps/linux/arc/bsd-_setjmp.S
	* libc/sysdeps/linux/arc/sigrestorer.S
	* libc/sysdeps/linux/arc/bsd-setjmp.S
	* libc/sysdeps/linux/arc/crt1.S
	* libc/sysdeps/linux/arc/Makefile
	* libc/sysdeps/linux/arc/sys
	* libc/sysdeps/linux/arc/sys/ucontext.h
	* libc/sysdeps/linux/arc/sys/procfs.h
	* libc/sysdeps/linux/arc/sys/user.h
	* libc/misc/statfs/statfs64.c
	* libpthread/linuxthreads.old/pthread.c
	* libpthread/linuxthreads.old/sysdeps/arc
	* libpthread/linuxthreads.old/sysdeps/arc/pt-machine.h
	* libpthread/linuxthreads.old/cancel.c
	* libpthread/linuxthreads/Makefile.in
	* libpthread/linuxthreads/sysdeps/unix/sysv/linux/arc
	* libpthread/linuxthreads/sysdeps/unix/sysv/linux/arc/sysdep-cancel.h
	* libpthread/linuxthreads/sysdeps/arc
	* libpthread/linuxthreads/sysdeps/arc/pspinlock.c
	* libpthread/linuxthreads/sysdeps/arc/stackinfo.h
	* libpthread/linuxthreads/sysdeps/arc/tls.h
	* libpthread/linuxthreads/sysdeps/arc/sigcontextinfo.h
	* libpthread/linuxthreads/sysdeps/arc/pt-machine.h
	* test/Rules.mak
	* include/elf.h
	* include/libc-symbols.h
	* Makerules
	* arc_config
	* Changelog.ARC
	* utils/ldd.c
	* Rules.mak


2008-09-05  Muhammad Khurram Riaz  <khurram.riaz@arc.com>

	* arc_config: Locale support enables
	* libc/string/arc/strcmp.S: features.h included
	* extra/locale/Makefile.in: header UCLIBC_CTYPE_HEADER path corrected
	* libc/sysdeps/linux/arc/bits/wchar.h: DELETED


2008-08-27  J"orn Rennecke  <joern.rennecke@arc.com>

	* ldso/ldso/powerpc/dl-sysdep.h (ELF_RESOLVE_TDEP): Define.
	* ldso/ldso/dl-hash.c (_dl_elf_hash): Use implementation with
	tighter loop core.
	(_dl_add_elf_hash_tab): Take COPY_HASH into account.
	* ldso/ldso/arc/dl-sysdep.h (__udivmodsi4): Extern hidden declaration.
	(do_rem): Use __udivmodsi4.
	* ldso/include/dl-hash.h (struct elf_resolve): Use ELF_RESOLVE_TDEP
	instead of ad-hoc #ifdefs.

2008-08-22 Muhammad Khurram Riaz <khurram.riaz@arc.com>
	* libc/sysdeps/linux/arc/bits/wchar.h
	WCHAR_MAX and WCHAR_MIN defined for 16bit; as compiler makes it 16bit type.

2008-07-22  J"orn Rennecke  <joern.rennecke@arc.com>

	* ldso/ldso/arc/resolve.S: Define __ASSEMBLY__
	* libc/sysdeps/linux/arc/vfork.S: Likewise.
	* libc/sysdeps/linux/arc/clone.S: Likewise.
	* libc/sysdeps/linux/arc/setjmp.S: Likewise.
	* libc/sysdeps/linux/arc/bsd-_setjmp.S: Likewise.
	* libc/sysdeps/linux/arc/bsd-setjmp.S: Likewise.
	* libc/sysdeps/linux/arc/crt1.S: Likewise.

2008-06-23 Mitesh Shah <Mitesh.Shah@arc.com>

	* Added __ASSEMBLY__ in vfork.S for subsequent kernel include files

2008-06-11  J"orn Rennecke  <joern.rennecke@arc.com>

	* libpthread/linuxthreads.old/sysdeps/arc/pt-machine.h:
	Only use register indirect or absolute addressing for ex.

2008-02-27  J"orn Rennecke  <joern.rennecke@arc.com>

	From Simon Spooner:
	* libc/string/arc/strcmp.S [__UCLIBC_HAS_LOCALE__] (strcoll): New alias.

2008-02-14  J"orn Rennecke  <joern.rennecke@arc.com>

	* libc/string/arc/uclibc-asm.h: New file.
	* libc/string/arc/memcmp.S: Include uclibc-asm.h.
	* libc/string/arc/strchr.S, libc/string/arc/strlen.S: Likewise.
	* libc/string/arc/strcmp.S, libc/string/arc/memcpy.S: Likewise.
	* libc/string/arc/memset.S: Likewise.
	* libc/string/arc/strcpy.S: Likewise.  Include newlib/strcpy-600.S.

2008-02-12  J"orn Rennecke  <joern.rennecke@arc.com>

	* libc/string/arc: New directory.

2007-12-06  J"orn Rennecke  <joern.rennecke@arc.com>

	* .config: New file, configuration for ARC.
	* extra/Configs/Config.arc: Adjust for 0.9.29.
	* include/libc-symbols.h (hidden_def): Define as hidden_weak.
	* Makerules: Be less silent.
	* ldso/ldso/arc/dl-startup.h: Updated for 0.9.29 (entire file).
	* ldso/ldso/arc/elfinterp.c: Updated for 0.9.29:
	(_dl_reltypes_tab) Remove.  (Btw this had incorrect indices.)
	(_dl_reltypes, debug_sym, debug_reloc): Remove.
	("ldso.h"): Include.
	* ldso/ldso/arc/dl-debug.h: New file.  Defines proper relocation
	names.
	* ldso/ldso/arc/dl-sysdep.h: Updated for 0.9.29:
	(elf.h): Include.
	(INIT_GOT): Fix cast type.
	(insert_arc_pc_rel_25): Cast REL argument to arc_read_me, arc_write_me
	and insert_arc_pc_rel_25.
	(elf_machine_dynamic): Fix gp name.
	(elf_machine_load_address): New function.
	(elf_machine_relative): New function.
	(libc/sysdeps/linux/arc/vfork.S): Adjust names to new scheme;
	provide weak aliases.
	* libc/sysdeps/linux/arc/brk.c (<unistd.h>): Include.
	(brk): Use libc_hidden_proto and libc_hidden_def.
	* libc/sysdeps/linux/arc/bits/uClibc_arch_features.h: New file.
	* libc/sysdeps/linux/arc/bits/syscalls.h: Include errno.h to
	get __set_errno definition.  Conditionalize this on
	#ifndef __ASSEMBLER__.
	* libc/sysdeps/linux/arc/bits/stackinfo.h: New file.
	* libc/sysdeps/linux/arc/__longjmp.S (__longjmp): Define.
	Add libc_hidden_def(__longjmp).
	* libc/sysdeps/linux/arc/crt0.S: Deleted.
	* libc/sysdeps/linux/arc/crt1.S: New file.
	* libc/sysdeps/linux/arc/Makefile: Updated for 0.9.29 (entire file).
	* libpthread/linuxthreads.old/sysdeps/arc/pt-machine.h: Copied from
	old linuxthreads/sysdeps/arc/pt-machine.h.

2007-10-24  J"orn Rennecke  <joern.rennecke@arc.com>

	* libc/sysdeps/linux/common/profil.c: New file.
