	;; $047CC
read_id_from_via:	
	moveq #0,%d1		
	btst #2,%d0
	beq VIA2
	moveal %a0@(8),%a1			; VIA1 address
	moveb %a1@($1E00),%d2		; VIA.vBufA
	lslw #8,%d2
	moveb %a1@($0600),%d2		; VIA.vDirA
	moveb %d2,%d1
	andb %a0@(-12),%d1			; VIA1A_MASK
	moveb %d1,%a1@($0600)		; VIA.vDirA
	moveb %a1@($1E00),%d1		; VIA.vBufA
	moveb %d2,%a1@($0600)		; VIA.vDirA
	lsrw #8,%d2
	moveb %d2,%a1@($1E00)		; VIA.vBufA
	lslw #8,%d1
	moveb %a1@,%d2				; VIA.vBufB
	lslw #8,%d2
	moveb %a1@($0400),%d2		; VIA.vDirB
	moveb %d2,%d1
	andb %a0@(-11),%d1			; VIA1B_MASK
	moveb %d1,%a1@($0400)		; VIA.vDirB
	moveb %a1@,%d1				; VIA.vBufB
	moveb %d2,%a1@($0400)		; VIA.vDirB
	lsrw #8,%d2
	moveb %d2,%a1@				; VIA.vBufB
VIA2:
	;; $04818
	swap %d1
	btst #11,%d0			
	beq DONE				
	moveal %a0@(44),%a1			; VIA2 address
	moveb %a1@($1E00),%d2		; VIA.vBufA
	lslw #8,%d2
	moveb %a1@($0600),%d2		; VIA.vDirA
	moveb %d2,%d1
	andb %a0@(-10),%d1			; VIA2A_MASK
	moveb %d1,%a1@($0600)		; VIA.vDirA
	moveb %a1@($1E00),%d1		; VIA.vBufA
	moveb %d2,%a1@($0600)		; VIA.vDirA
	lsrw #8,%d2
	moveb %d2,%a1@($1E00)		; VIA.vBufA
	lslw #8,%d1
	moveb %a1@,%d2				; VIA.vBufB
	lslw #8,%d2
	moveb %a1@($0400),%d2		; VIA.vDirB
	moveb %d2,%d1
	andb %a0@(-9),%d1			; VIA2B_MASK
	moveb %d1,%a1@($0400)		; VIA.vDirB
	moveb %a1@,%d1				; VIA.vBufB
	moveb %d2,%a1@($0400)		; VIA.vDirB
	lsrw #8,%d2
	moveb %d2,%a1@				; VIA.vBufB
DONE:
	;; $04864
	ret			
