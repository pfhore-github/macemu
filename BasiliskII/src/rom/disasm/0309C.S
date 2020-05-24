	;; $0309C
update_rom_flag:	
	movel %a1@(28),%d1			;ROM FLAG(from MACHINE)
	bne DONE
	movel %a0@(-16),%d1			;ROM FLAG(from MOTHERBOARD)
DONE:
	;; $030A6
	bclr #27,%d7
	ret
