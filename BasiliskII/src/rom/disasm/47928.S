	;; $47928
recv_from_pb:	
	moveq #2,%d6
	movew 0x61E0,%d4
	bclr #31,%d4
	btst #1,%a0@(-15)
	beq 0x47976
	moveal %a0@(52),%a3
	bset #31,%d4
	bclr #4,%a2@(0x1600)
	tstb %a2@(0x1400)
	bclr #2,%a3@
	tstb %a2@
	btst #1,%a3@
	dbeq %d4,0x47950
	bne 0x479a0
	bset #2,%a3@
	movew #97,%d4
47964:
	tstb %a2@			; 4a12
47966:	 btst #1,%a3@			; 0813 0001
4796a:	 dbne %d4,0x47964		; 56cc fff8
4796e:	 beq 0x479a0			; 6730
47970:	 moveb %a2@(5120),%d3		; 162a 1400
47974:	 bra 0x4799e			; 6028
47976:
	clrb %a3@(1536)		; 422b 0600
4797a:	 tstb %a2@			; 4a12
4797c:	 btst #1,%a3@			; 0813 0001
47980:	 dbeq %d4,0x4797a		; 57cc fff8
47984:	 bne 0x479a0			; 661a
47986:	 bclr #2,%a3@			; 0893 0002
4798a:	 moveb %a3@(512),%d3		; 162b 0200
4798e:	 movew #97,%d4			; 383c 0061
47992:	 tstb %a2@			; 4a12
47994:	 btst #1,%a3@			; 0813 0001
47998:	 dbne %d4,0x47992		; 56cc fff8
4799c:	 beq 0x479a0			; 6702
4799e:	 moveq #0,%d6			; 7c00
479a0:	 bset #2,%a3@			; 08d3 0002
479a4:	 tstl %d4			; 4a84
479a6:	 bmi 0x479ae			; 6b06
479a8:	 clrb %a2@(1536)		; 422a 0600
479ac:	 bra 0x479b4			; 6006
479ae:	 bset #4,%a2@(5632)		; 08ea 0004 1600
479b4:	 tstl %d6			; 4a86
479b6:	 retx %a5			; 4ed5
