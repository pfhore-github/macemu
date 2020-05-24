init_mmu:	
	;; $04052
	subl %d0,%d0
	bset #31,%d0
	movec %d0,%cacr
	movec %cacr,%d0
	btst #31,%d0
	beq MC68030
    
	cinva bc
	subl %d0,%d0
	movec %d0,%cacr
	movec %d0,%tc
	movel 0x807FC040,%d0
	movec %d0,%dtt0
	movel 0x500FC040,%d0
	movec %d0,%dtt1
	bra END
MC68030:
	movel 0x2000,%d0
	movec %d0,%cacr
	movec %cacr,%d0
	tstl %d0
	beq END
	lea DATA, %a0
	pmove %a0@,%tc
END:
	ret
DATA:
	;; $40A4
	.long 0
	.long $4E7A1002
	.long $908008C0
	;; to be continued...
	
