has_ether:
	tstb %a1@
	bset #7,%a1@
	cmpw %d1,%d1
	ret
