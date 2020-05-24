	;; $45EA2
easc_beep:	
	movel 0xFF0000,%d1
	lea 0x45f3c,%a5
	clrw %d1
	moveq #0,%d0
	movew %d2,%d0				;D2.L
	bne 0x45eba
	addil 0x10000,%d3
45eba:
	addl %d0,%d3
	swap %d2
	movew %d2,%d0				;D2.H
	bne 0x45ec8
	addal 0x10000,%a3
45ec8:
	addal %d0,%a3
	swap %d2
	movew %d4,%d0				;D4.L
	bne 0x45ed6
	addal 0x10000,%a0
45ed6:
	addal %d0,%a0
	swap %d4
	movew %d4,%d0				;D4.H
	bne 0x45ee4
	addil 0x10000,%d6
45ee4:
	addl %d0,%d6
	swap %d4
	movel %d3,%d0
	andl %d1,%d0
	swap %d0
	moveb %a5@(%d0:w),%d1
	movel %a3,%d0
	andl %d1,%d0
	swap %d0
	moveb %a5@(%d0:w),%d0
	addw %d0,%d1
	movel %a0,%d0
	andl %d1,%d0
	swap %d0
	moveb %a5@(%d0:w),%d0
	addw %d0,%d1
	movel %d6,%d0
	andl %d1,%d0
	swap %d0
	moveb %a5@(%d0:w),%d0
	addw %d0,%d1
	moveq #0,%d0
	movew %d1,%d0
	subiw #512,%d0
	extl %d0
	mulsw %d5,%d0
	swap %d0
	addiw #128,%d0
	moveb %d0,%fp@
	moveb %d0,%fp@(1024)
	ret							; bra $BD3EC
