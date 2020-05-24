init_hw:
	moveaw 0x2600,%sp
	moveal %fp,%a4			
	lea ROM_BASE,%a0		; $03DE8
	movel %a0,%d0			
rom_base_loop:
	;; $02E0C
	cmpl %a0@-,%d0
	beq rom_base_done
	tstl %a0@
	bne rom_base_loop
	lea 0x8,%a0
	movel %d0,%a0@
rom_base_done:
	;; $02E1A
	subqw #8,%a0
	movec %a0,%vbr
get_hw_info_loop:
	;; $02E20
	moveq #0,%d2
	call get_hw_info		; $02F64
	btst #0,%d0
	beq get_hw_info_loop
	
	lea %pc,%a2
	call remap_rom			; jmp $04AFE
	
	addal %d3,%a4
	call init_hw2				; bra $04A5E
	
	btst #18,%d0
	beq 0x2e56			; 670a           
	moveal %a0@(72),%a3		; 2668 0048      
	moveb #13,%a3@(516)		; 177c 000d 0204 
02e56:
	btst #19,%d0			; 0800 0013      
	beq 0x2e72			; 6716           
	moveal %a0@(76),%a3		; 2668 004c      
	movew #-3073,%d3		; 363c f3ff      
	moveq #15,%d4			; 780f           
02e66:
	moveb %d3,%a3@			; 1683           
	lsrw #1,%d3			; e24b           
	dbf %d4,0x2e66			; 51cc fffa      
	moveb %d3,%a3@(16)		; 1743 0010      
02e72:
	bral 0x4866			; 60ff 0000 19f2 
02e78:	 retx %a4			; 4ed4           

	
ROM_BASE_0:
	;; $3DD0
	.long 0x00A03DE8
	.long 0x40003DE8
	.long 0x40803DE8
	.long 0x00003DE8
ROM_BASE:
	nop
