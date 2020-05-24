ba182:
	;;  $BA182
	moveal %fp,%a5
	movel %d0,%d1
	tstw %d2
	beq 0xba1c6
	moveq #4,%d0
	subw %d2,%d0
	muluw #8,%d0
	roll %d0,%d1
	btst #20,%d7
	beq 0xba19c
	aslw #1,%d2
ba19c:
	subqw #1,%d2
LOOP:
	;; $BA19E
	btst #20,%d7
	bne FLAG20_ELSE
	roll #8,%d1
	moveb %d1,%d0
	bra FALG20_ENDIF
FLAG20_ELSE:
	;; $BA1AA
	roll #4,%d1
	moveb %d1,%d0
	andib #15,%d0
	call 0xba174
FLAG20_ENDIF:
	;; $BA1BA
	call 0xb9fda
	dbf %d2,LOOP
ba1c6:
	btst #23,%d7			; 0807 0017
ba1ca:	 beq 0xba1e4			; 6718
ba1cc:	 moveb #13,%d0			; 103c 000d
ba1d0:	 call 0xb9fda			; 4dfa 0006 4efa fe04
ba1d8:	 moveb #10,%d0			; 103c 000a
ba1dc:	 call 0xb9fda			; 4dfa 0006 4efa fdf8
ba1e4:	 retx %a5			; 4ed5
