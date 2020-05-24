get_scc_addr:	
	;; $9F46
	movel %fp,%d3
	moveq #0,%d2
	call getHWInfo2
	moveal %d3,%fp
	moveal %a0@(8),%a2
	btst #17,%d0
	beq NON_IOP
	moveal %a0@(68),%a3
	moveq #0,%d3
	bra DONE
NON_IOP:
	;; $B9F6C
	moveal %a0@(12),%a3
	movel %a0@(16),%d3
	subl %a3,%d3

DONE:	
	;; $B9F76
	ret
