b98bc:
	moveq #0,%d2
	call getHWInfo2
	moveal %a0@(8),%a2			;VIA1
	btst #24,%d2
	beq SKIP_SEND_PB_KEY
	call send_to_pb_key	; $47826
	;; $B98E4
SKIP_SEND_PB_KEY:	
	btst #26,%d7
	bne SCC_MODEM_TEST
	moveq #0,%d2
	call getHWInfo2
	btst #12,%d0
	beq SCC_MODEM_TEST
	moveal %a0@(48),%a3			; ASC
	moveal %a0@(8),%a5			; VIA1
	call play_chime_4		; $45C52
	;; $B9916
SCC_MODEM_TEST:	
	moveq #0,%d5
	call get_scc_addr	; $B9F46
	call init_modem_port		; $B9F78
LOOP:
	;; $B9928
	call read_modem_port		; $BA0EA
	tstw %d0
	bmi 0xb9a4e
	andib 0x7F,%d0
	cmpib 0x2A,%d0
	bne NON_2A
	bset #19,%d7
	bra 0xb9a4e
NON_2A:	
	;; $B9948
	btst #19,%d7
	bne HANDLER1
	cmpib #33,%d0
	bne 0xb9a46
	bsr 0xba578

HANDLER1:	
	;; $B997C
	bclr #19,%d7
	moveq #0,%d1
	moveq #0,%d2
	lea 0xb99b0,%a5
LOOP:
	;; $B9988
	movew %a5@+,%d2
	beq 0xb9a46
	cmpb %d2,%d0
	beq BREAK
	addql #2,%a5
	bra LOOP
BREAK:
	;; $B9996
	movel %a5,%d5
	lsrw #8,%d2
	call 0xba138
	moveal %d5,%a5
	moveb %a5@(-1),%d5
	swap %d5
	movew %a5@,%d0
	jmp %a5@(%d0:w)
