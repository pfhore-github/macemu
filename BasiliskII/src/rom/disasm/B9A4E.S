b9a4e:
	btst #16,%d7
	beq DONE
	btst #22,%d7
	beq 0xb9a82	
	btst #5,%a2@(6656)			; VIA.IFR
	beq DONE
	moveb 0x20,%a2@(7168)		; VIA.IER
	moveb 0xff,%a2@(4096)		; VIA.TIMER2_L
	moveb 0xff,%a2@(4608)		; VIA.TIMER2_H
	subqw #1,%d4
	bpl DONE
	movew #12,%d4
b9a82:
	lea 0xba010,%a1								; "*APPLE*"
	call send_cstr_to_modem_port			; $B9FF6
	bclr #23,%d7
	bset #20,%d7
	movel %d6,%d0
	moveq #4,%d2
	call 0xba182
	movew %d7,%d0
	moveq #2,%d2
	call 0xba182
	bclr #20,%d7
	moveb #42,%d0
	call 0xb9fda
	call 0xb9f46
b9ac6:	 lea 0xba01b,%a1		; 43fa 0553
b9aca:	 lsrw #8,%d2			; e04a
b9acc:	 extw %d2			; 4882
b9ace:	 moveb %a1@(%d2:w),%d0; 1031 2000
b9ad2:	 call 0xb9fda			; 4dfa 0006 4efa 0502
b9ada:	 lea 0xba04c,%a1		; 43fa 0570
b9ade:	 call 0xb9ff6			; 4dfa 0006 4efa 0512
b9ae6:	 btst #22,%d7			; 0807 0016
b9aea:	 bne DONE			; 6604
b9aec:	 bclr #16,%d7			; 0887 0010
DONE:	
	;; $B9AF0
	bra 0xb9928			; 6000 fe36
