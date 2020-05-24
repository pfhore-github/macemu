	;; $04BE4
get_model_id:	
	lea 0x5ffffffc,%a2		; MACHINE_ID
	movel %a2@,%d0	
	moveq #0,%d1
LOOP:
	movel %d1,%a2@
	cmpl %a2@,%d0
	bne DONE
	notl %d1
	bne LOOP
	swap %d0
	cmpiw $A55A,%d0
	bne DONE
	swap %d0
	cmpw %d0,%d0
DONE:
	;; $04C04
	ret
