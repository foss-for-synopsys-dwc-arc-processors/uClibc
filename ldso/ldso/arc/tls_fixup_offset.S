	.balign 4
__tls_fixup_offset:
	; fetch pieces of bbit0 RB,31,0f insn with register number
	#ifdef __BIG_ENDIAN__
	ldb r12,[blink,-8] ; 00001bbb
	push_s r0
	ldb r0, [blink,-6] ; 0BBB0111
	#else
	ldb r12,[blink,-7]
	push_s r0
	ldb r0, [blink,-5]
	#endif
	push_s r1
	ld r1,[__ARC_RTP__,0] ; fetch dtv
0:	add2 r12,pcl,r12
	push_s r2
	ld r2,[pcl,_dl_tls_generation@gotpc]
	push_s r3
	add_s r0,r0,r12
	ld_s r12,[r1,4] ; dtv generation
	j_s.d [r0]
	ld_s r2,[r2] ; _dl_tls_generation
	.balign 4
1:

	; Compensate for the offset of 15 in the extracted register number,
	; and the difference between the pcl value used and .Lreg_access_tab .
	.rep 9 ; 15 - ((1b - 0b + 2) / 4)
	nop
	.endr
	
.Lreg_access_tab:
	ld_s r3,[sp,12]; saved r0
	b_s .Loaded
	ld_s r3,[sp,8] ; saved r1
	b_s .Loaded
	ld_s r3,[sp,4] ; saved r2
	b_s .Loaded
	b_s .Loaded ; r3 := r3
	nop_s
	mov_s r3,r4
	b_s .Loaded
	mov_s r3,r5
	b_s .Loaded
	mov_s r3,r6
	b_s .Loaded
	mov_s r3,r7
	b_s .Loaded
	j_s.d [blink]
	mov_s r0,r12
	j_s.d [blink]
	mov_s r1,r12
	j_s.d [blink]
	mov_s r2,r12
	j_s.d [blink]
	mov_s r3,r12
	j_s.d [blink]
	mov_s r4,r12
	j_s.d [blink]
	mov_s r5,r12
	j_s.d [blink]
	mov_s r6,r12
	j_s.d [blink]
	mov_s r7,r12
	mov_s r3,r8
	b_s .Loaded
	mov_s r3,r9
	b_s .Loaded
	mov_s r3,r10
	b_s .Loaded
	mov_s r3,r11
	b_s .Loaded
	mov_s r3,r12 ; can't actually work because of plt
	b_s .Loaded
	mov_s r3,r13
	b_s .Loaded
	mov_s r3,r14
	b_s .Loaded
	mov_s r3,r15
	b_s .Loaded
	j_s.d [blink]
	mov_s r8,r12
	j_s.d [blink]
	mov_s r9,r12
	j_s.d [blink]
	mov_s r10,r12
	j_s.d [blink]
	mov_s r11,r12
	j_s [blink]
	nop_s
	j_s.d [blink]
	mov_s r13,r12
	j_s.d [blink]
	mov_s r14,r12
	j_s.d [blink]
	mov_s r15,r12
	mov_s r3,r16
	b_s .Loaded
	mov_s r3,r17
	b_s .Loaded
	mov_s r3,r18
	b_s .Loaded
	mov_s r3,r19
	b_s .Loaded
	mov_s r3,r20
	b_s .Loaded
	mov_s r3,r21
	b_s .Loaded
	mov_s r3,r22
	b_s .Loaded
	mov_s r3,r23
	b_s .Loaded
	j_s.d [blink]
	mov_s r16,r12
	j_s.d [blink]
	mov_s r17,r12
	j_s.d [blink]
	mov_s r18,r12
	j_s.d [blink]
	mov_s r19,r12
	j_s.d [blink]
	mov_s r20,r12
	j_s.d [blink]
	mov_s r21,r12
	j_s.d [blink]
	mov_s r22,r12
	j_s.d [blink]
	mov_s r23,r12
	mov_s r3,r24
	b_s .Loaded
	mov_s r3,r25
	b_s .Loaded
	mov_s r3,gp
	b_s .Loaded
	mov_s r3,fp
	b_s .Loaded
.Lfetch:
	asl_s r3,r3,2
	ld_s r2,[r3]
	ld_s r3,[r3,4]
	b.d .Lfetched
	ld.as r12,[r1,r2]
	nop_s
	j_s.d [blink]
	mov_s r24,r12
	j_s.d [blink]
	mov_s r25,r12
	j_s.d [blink]
	mov_s gp,r12
	j_s.d [blink]
	mov_s fp,r12
.Loaded:
	brne r2,r12,.Lrefresh_dtv
	bbit1 r3,30,.Lfetch
	bmsk r2,r3,14
	ld.as r12,[r1,r2]
	lsr_s r3,r3,14
	bmsk_s r3,r3,16
.Lfetched:
	sub r3,r3,__ARC_RTP__
	add_s r0,r0,4
	brlt r12,0,.Lalloc_slot
	add_s r12,r12,r3
.Lassign_result:
	pop_s r3
	pop_s r2
	pop_s r1
	j_s.d [r0]
	pop_s r0

.Lfetch2:
	asl_s r3,r3,2
	ld_s r2,[r3]
	ld_s r3,[r3,4]
	b_s .Lfetched2

.Lrefresh_dtv:
	bbit1 r3,30,.Lfetch2
	bmsk r2,r3,14
	lsr_s r3,r3,14
	bmsk_s r3,r3,16
.Lfetched2:
	add_s r0,r0,4
.Lalloc_slot:
	st.a r4,[sp,-4]
	st.a r5,[sp,-4]
	st.a r6,[sp,-4]
	st.a r7,[sp,-4]
	st.a r8,[sp,-4]
	st.a r9,[sp,-4]
	st.a r10,[sp,-4]
	st.a r11,[sp,-4]
	st.a lp_count,[sp,-4]
	lr r1,[status32]
        lr r2,[lp_start]
        lr r3,[lp_end]
	push_s r1
        push_s r2
        push_s r3
	push_s blink
	push_s r0
	mov_s r0,r2
	bl.d __tls_get_addr
	mov_s r1,r3
	sub r12,r0,__ARC_RTP__
	pop_s r0
        pop_s blink
        pop_s r3
        pop_s r2
	pop_s r1
        pop_s r12
	ld.ab r11,[sp,4]
	ld.ab r10,[sp,4]
        sr r3,[lp_end]
        sr r2,[lp_start]
	flag r1
        mov lp_count,r12
	ld.ab r9,[sp,4]
	ld.ab r8,[sp,4]
	ld.ab r7,[sp,4]
	ld.ab r6,[sp,4]
	ld.ab r5,[sp,4]
	b.d .Lassign_result
	ld.ab r4,[sp,4]
