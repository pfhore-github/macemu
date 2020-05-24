	;; $046D8
has_rbv:	
	moveb %a2@,%d1
	rolw #8,%d1
	st %d1
	moveb %d1,%a2@
LOOP:
	;; $046E0
	negb %d1
	moveb %d1,%a2@
	addb %d1,%d1
	beq LOOP_DONE
	negb %d1
	moveb %a2@,%d2
	eorb %d1,%d2
	andib 0x9F,%d2
	beq LOOP
LOOP_DONE:	
	;; $046F4
	rorw #8,%d1
	moveb #127,%a2@
	moveb %d1,%a2@
	lsrw #8,%d1
	ret
