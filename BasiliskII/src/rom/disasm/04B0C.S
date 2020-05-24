check_flag:	
	;; $04B0C
	btst #28,%d2				; HAS_FPU
	beq ETHER_TEST
	moveal %fp,%a2
	call has_fpu			; bra $04640
	moveal %a2,%fp
	beq ETHER_TEST
	bclr #28,%d2				; HAS_FPU
ETHER_TEST:
	;; $04B20
	bset #27,%d7				; ENABLE HW TEST
	moveal %sp,%a5
	bclr #23,%d0				; ETHERNET
	beq LOWMEM
	movel %a1,%d1
	moveal %a0@(92),%a1			; .ether
	moveal %fp,%a2
	call test_ether				; $047B0
	moveal %a2,%fp
	moveal %d1,%a1
	bne DONE
	bset #23,%d0			; ETHERNET
LOWMEM:
	;; $04B46
	bclr #29,%d0			; LOWMEM
	beq DONE
	movel %a1,%d1			
	moveal %a0@(116),%a1		; .secret
	moveal %fp,%a2
	call has_LMG				; TODO: $047BA
	moveal %a2,%fp
	moveal %d1,%a1
	bne DONE
	bset #29,%d0			; LOWMEM
DONE:	
	;; $4B66
	ret							; bra $0309C
