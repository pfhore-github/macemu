ba174:
	cmpib #10,%d0
	blt 0xba17c
	addqb #7,%d0
ba17c:
	addib 0x30,%d0
	ret
