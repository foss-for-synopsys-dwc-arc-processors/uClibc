___gnu_compiled_c:

	.section .init
	.align 4
	.global	_init
	.type	 _init,@function
	; EPILOGUE
	ld.ab fp,[sp,4]
	ld blink,[sp,0]
	j.d [blink]
	add sp,sp,4
;	.size	 _init,.-_init

	.section .fini
	.align 4
	.global	_fini
	.type	 _fini,@function
	; EPILOGUE
	ld.ab fp,[sp,4]
	ld blink,[sp,0]
	j.d [blink]
	add sp,sp,4
;	.size	 _fini,.-_fini
