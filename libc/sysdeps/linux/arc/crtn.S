___gnu_compiled_c:
	
	.section .init
	.align 4
	.global	_init
	.type	 _init,@function
	; EPILOGUE
#ifdef __A4__
	ld	fp, [sp]
	add	sp, sp, 4
#else
	ld.ab fp,[sp,4]
#endif
	ld blink,[sp,0]
	j.d [blink]
	add sp,sp,4
.Lfe2:
	.size	 _init,.Lfe2-_init
	
	.section .fini
	.align 4
	.global	_fini
	.type	 _fini,@function
	; EPILOGUE
#ifdef __A4__
	ld	fp, [sp]
	add	sp, sp, 4
#else
	ld.ab fp,[sp,4]
#endif
	ld blink,[sp,0]
	j.d [blink]
	add sp,sp,4
.Lfe3:
	.size	 _fini,.Lfe3-_fini
	
