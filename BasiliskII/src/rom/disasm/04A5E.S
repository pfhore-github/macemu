init_hw2:
	;; $04A5E
	addal %d3,%sp
	lea ROM_BASE,%a0			; $03DE8
	movel %a0,%d0
rom_base_loop:	
	;; $04A66
	cmpl %a0@-,%d0
	beq rom_base_done
	tstl %a0@
	bne rom_base_loop
	lea 0x8,%a0
	movel %d0,%a0@
rom_base_done:
	;; $04A74
	subqw #8,%a0
	movec %a0,%vbr
get_hw_info_loop:
	;; $04A7A
	moveq #0,%d2
	call get_hw_info			; $02F64
	btst #0,%d0
	beq get_hw_info_loop
	call 0x2e8c			; 4dfa 0006 4efa e3fc
	ret					; bra $02E46
