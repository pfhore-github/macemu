	bne NON_SONORA
	movew #127,%d5
INIT_LOOP:
	;; $45D3C
	movel 0x80808080,%fp@
	movel 0x80808080,%fp@(1024)
	dbf %d5,INIT_LOOP
NON_SONORA:
	;; $45D4E
	movel 0x4000,%d5
	movew %a4@(8),%d2
	tstw %a4@
	bne LONG_SND
	swap %d2
	movew %a4@(10),%d2
	swap %d2
	movew %a4@(12),%d4
	swap %d4
	movew %a4@(14),%d4
	swap %d4
	bra SHORT_SND
LONG_SND:	
	;; $45D72
	swap %d5
	movew %a4@(2),%d5
WAV1_LOOP:
	;; $45D78
	swap %d5
	call 0x45ea2
	call 0x45e3c
	swap %d5
	subqw #1,%d5
	bne WAV1_LOOP
	swap %d2
	movew %a4@(10),%d2
	swap %d2
	movew %a4@(4),%d5
WAV2_LOOP:
	;; $45D9C
	swap %d5
	call 0x45ea2
	call 0x45e3c
	swap %d5
	subqw #1,%d5
	bne WAV2_LOOP
	movew %a4@(12),%d4
	movew %a4@(6),%d5
WAV3_LOOP:
	;; $45DBC
	swap %d5
	call 0x45ea2
	call 0x45e3c
	swap %d5
	subqw #1,%d5
	bne WAV3_LOOP
	swap %d4
	movew %a4@(14),%d4
	swap %d4
	bra FINAL_WAV
SHORT_SND:
	;; $45DDE
	swap %d5
FINAL_WAV:	
	;; $45DE0
	movew #33,%d5
WAV4_LOOP:
	;; $45DE4
	swap %d5
	call 0x45ea2
	call 0x45e3c
	subiw #512,%d5
	swap %d5
	cmpib #-80,%fp@(2048)
	subqw #1,%d5
	bne WAV4_LOOP
	call 0xbd470
45e0e:
	bne SHUTDOWN_ASC
WAIT_DONE_LOOP:
	;; $45E10
45e10:
	bra 0xbd494			; 60ff 0007 7682
	cmpib #3,%d0			; 0c00 0003
	bne WAIT_DONE_LOOP
SHUTDOWN_ASC:	
	;; $45E1E
	clrb %fp@(0x801)
	clrb %fp@(0x806)
	bra 0xbd482			; 60ff 0007 765a
	bne DONE
	clrb %fp@(0xF09)
	clrb %fp@(0xF29)
DONE:	
	moveal %fp,%a3
	moveal %a2,%fp
	ret
