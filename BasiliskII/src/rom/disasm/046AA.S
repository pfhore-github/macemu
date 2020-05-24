	;; $046AA
has_via:	
	moveq #0,%d2
has_via_with_offset:
	;; $046AC
	tstb %a2@(0,%d2:l)
	moveb %a2@,%d1
	rolw #8,%d1
	st %d1
	moveb %d1,%a2@
LOOP:
	;; $046B8
	negb %d1
	moveb %d1,%a2@
	addb %d1,%d1
	beq LOOP_DONE
	negb %d1
	cmpb %a2@(0,%d2:l),%d1
	bne LOOP_DONE
	cmpb %a2@,%d1
	beq LOOP
LOOP_DONE:
	;;  $046CC
	rorw #8,%d1
	moveb #127,%a2@
	moveb %d1,%a2@
	lsrw #8,%d1
	ret
