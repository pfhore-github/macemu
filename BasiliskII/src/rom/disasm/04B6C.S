test_model_id:
	;; $04B6C
	bset #27,%d7			
	moveal %sp,%a5			
	moveal %fp,%a1			
	call get_model_id			; $04BE4
	moveal %a1,%fp			
	beq HAS_MODEL_ID
	bra 04B6C_FAIL				; $02F6A
HAS_MODEL_ID:
	;; $04B86
	lea NEWER_MACHINE_ADDR(-4), %a2	; %A79C0-4
LOOP:
	;; $04B90
	addqw #4,%a2
	movel %a2@,%d1
	beq HALT
	lea %a2@(0,%d1:l),%a1;
	cmpw %a1@(68),%d0	 ; A1(68) = machine_id
	bne LOOP
	btst #11,%d0				; VIA MID flag
	beq DONE
	moveal %a1@,%a0				; A1(0) = MB_ADDR_offset
	addal %a1,%a0
	movel %a0@(-20),%d0			; A0(-20) = HW_INIT_FLG
	moveal %fp,%a3
	call read_id_from_via		; $047CC
	moveal %a3,%fp
	movel %a2@,%d0
	lea %a2@(0,%d0:l),%a1
	andl %a1@(32),%d1			; model_id mask
	cmpl %a1@(36),%d1			; model_id value
	beq DONE
	moveal %a2,%a3
	moveal %fp,%a1
	call get_model_id
	moveal %a1,%fp
	moveal %a3,%a2
	bra LOOP
DONE:	
	;; $04BDC
	bra 04B6C_SUCCESS			; $2F82
HALT:
	;; $04BE2
	bra HALT
