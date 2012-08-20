___gnu_compiled_c:
	
	.section .init
	.align 4
	.global	_init
	.type	 _init,@function
_init:
	; BEGIN PROLOGUE ; vars= 0, regs= 0, args= 0, extra= 8
	st.a blink,[sp,-4]
	st.a fp,[sp,-4]
	mov fp,sp
	; END PROLOGUE
	
	.align 4
	
	
	.section .fini
	.align 4
	.global	_fini
	.type	 _fini,@function
_fini:
	; BEGIN PROLOGUE ; vars= 0, regs= 0, args= 0, extra= 8
	st.a blink,[sp,-4]
	st.a fp,[sp,-4]
	mov fp,sp
	; END PROLOGUE
	.align 4
	
	
	.ident	"GCC: (GNU) 2.95.3-A700-minimal-branch"
