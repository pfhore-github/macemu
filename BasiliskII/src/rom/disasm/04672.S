	;; $04672
test_iifx_exp1:	
	tstb %a2@
	tstb %a2@
	tstb %a2@
	tstb %a2@
	st %a2@(16)
	moveb %a2@,%d2
	st %a2@(16)
	st %a2@
	clrb %a2@(16)
	moveq #1,%d1
	andb %a2@,%d1
	seq %d1
	beq 0x46a0
	st %a2@(16)	
	clrb %a2@
	st %a2@(16)	
	moveq #1,%d1
	andb %a2@,%d1
046a0:
	st %a2@(16)	
	moveb %d2,%a2@
	tstb %d1
	ret
