has_fpu:	
	;; $04640
	movec %vbr,%d1
	subil #36,%d1
	movec %d1,%vbr
	moveq #1,%d1
	lea RET,%fp	;$0465A
	fnop
	moveq #0,%d1
RET:
	;; $0465A
	moveal %d1,%fp
	movec %vbr,%d1
	addil #36,%d1
	movec %d1,%vbr
	movel %fp,%d1
	tstb %d1
	ret							; bra 0x4B18
