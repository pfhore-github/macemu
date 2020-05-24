	moveal %fp,%a5			; 2a4e
	tstw %d2			; 4a42
	beq DONE
	moveq #0,%d1			; 7200
	btst #20,%d7			; 0807 0014
	beq NO_SHIFT
	aslw #1,%d2			; e342
NO_SHIFT:
	;; $BA148
	subqw #1,%d2			; 5342
LOOP:
	;; $BA14A
	call read_modem_port		; $BA0EA
	tstw %d0
	bmi LOOP1
	btst #20,%d7
	beq 0xba16a
	call 0xba11a
	lsll #4,%d1
	orb %d0,%d1
	bra CONTINUE
ba16a:
	lsll #8,%d1
	moveb %d0,%d1
CONTINUE:
	;; $BA16E
	dbf %d2,LOOP
DONE:
	;; $BA172
	ret
