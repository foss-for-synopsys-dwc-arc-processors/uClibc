#
# For a description of the syntax of this configuration file,
# see extra/config/Kconfig-language.txt
#
# Based on Config.arm
#
# Amit Shah
#
# uClibc-0.9.26 support Added by Robin
# uClibc-0.9.29 support added by Joern Rennecke

config TARGET_ARCH
	string
	default "arc"

config FORCE_OPTIONS_FOR_ARCH
	bool
	default y
	select ARCH_LITTLE_ENDIAN

config ARCH_CFLAGS
	string

config ARCH_LDFLAGS
	string

config LIBGCC_CFLAGS
	string

choice
	prompt "Target Processor Type"
	default CONFIG_ARCH_ARC_A7
	help
	  Select this option to select the A7 version of the ARC processor.

config CONFIG_ARCH_ARC_A7
	bool "ARC ARC700 processor"
config CONFIG_ARCH_ARC_A5
	bool "ARC A5 processor"
config CONFIG_ARCH_ARC_A4
	bool "ARC A4 processor"
	

endchoice

config ARCH_HAS_NO_MMU
	bool
	default n if CONFIG_ARCH_ARC_A7
	default y if CONFIG_ARCH_ARC_A5
	default y if CONFIG_ARCH_ARC_A4

config ARCH_HAS_MMU
	bool
	default n if ARCH_HAS_NO_MMU

config UCLIBC_HAS_MMU
	bool
	default y if ARCH_HAS_MMU

config HAVE_NO_PIC
	bool
	default y if CONFIG_ARCH_ARC_A5
	default y if CONFIG_ARCH_ARC_A4
	
config ARCH_HAS_C_SYMBOL_PREFIX
	bool "Add a C Prefix"
	default n 


#source "extra/Configs/Config.in.arch"

#source "extra/Configs/Config.in"
