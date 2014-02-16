; __tls_get_addr_refresh_dtv, called by (hidden) __tls_get_gd_addr
; return value: r0, the address of the tls variable
; already pushed: r1,r2,r3
; need to push: r4-r8, lp_count
; may clobber: r9,flags
; r10: module index
; r11: -1 id we need to call dl_update_slotinfo@plt
; r12: variable intra-module offset

	.section .text
	.balign 4
	.global __tls_get_addr_refresh_dtv
	.type __tls_get_addr_refresh_dtv, @function
__tls_get_addr_refresh_dtv:
	st.a r4,[sp,-4]
	st.a r5,[sp,-4]
	st.a r6,[sp,-4]
	st.a r7,[sp,-4]
	st.a r8,[sp,-4]
	st.a lp_count,[sp,-4]
	lr r2,[lp_start]
	lr r3,[lp_end]
	push_s r2
	push_s r3
	push_s blink
	lsr r0,r10
	bl __tls_get_addr
	mov_s r1,r12
	pop_s blink
	pop_s r3
	pop_s r2
	pop_s r12
	sr r3,[lp_end]
	sr r2,[lp_start]
	mov lp_count,r12
	ld.ab r8,[sp,4]
	ld.ab r7,[sp,4]
	ld.ab r6,[sp,4]
	ld.ab r5,[sp,4]
	ld.ab r4,[sp,4]
	pop_s r3
	pop_s r2
	j_s.d [blink]
	pop_s r1
	.size __tls_get_addr_refresh_dtv,.-__tls_get_addr_refresh_dtv
