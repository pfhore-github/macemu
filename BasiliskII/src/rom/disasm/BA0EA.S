read_modem_port:
	;; $BA0EA
	movew 0x8000,%d0
	btst #17,%d7
	beq DONE
	btst #0,%a3@(2)
	beq DONE
	moveq #1,%d0
	moveb %d0,%a3@(2,%d3:l)
	moveb %a3@(2),%d0
	andib #112,%d0
	beq RET
	moveb #48,%a3@(2,%d3:l)
RET:
	;; $BA112
	lsll #8,%d0
	moveb %a3@(6),%d0
DONE:
	;; $BA118
	ret
