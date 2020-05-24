	;; $4AA98
sad_mac:
	moveaw #9728,%sp
	moveq #0,%d2
	call 0x47752
	btst #26,%d7
	bne 0x4aac8
	moveal %a0@(48),%a3
	moveal %a0@(8),%a5
	call 0x45C1C
	bra 0xb98bc			; 60ff 0006 edf8
4aac8:
	btst #1,%d0
	beq 0x4aaec
	lea 0x4aaec,%fp
	bset #27,%d7
	moveal %a0@(4),%a1
	moveml %a1@+,%d0/%a0
	cmpil 0xAAAA5555,%d0
	bne 0x4aaec
	lea 0x4aaec,%a1
	call %a0
4aaec:
	bclr #27,%d7
	bset #16,%d7
	bset #22,%d7
	movew #12,%d4
	call 0xBA090
	movec %cacr,%d0
	bset #15,%d0
	movec %d0,%cacr
	movec %cacr,%d0
	btst #15,%d0
	beq MC68030
	cinva ic
	bra DONE
MC68030:
	;; $4AB24
	movec %cacr,%d0
	bset #3,%d0
	movec %d0,%cacr
	movec %cacr,%d0
	orib 0x11,%d0	
	movec %d0,%cacr
DONE:
	;; $4AB3C
	bra 0xb98bc
