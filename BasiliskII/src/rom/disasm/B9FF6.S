b9ff6:
	moveal %fp,%a0			; 204e
LOOP:
	;; $B9FF8
	moveb %a1@+,%d0
	beq DONE
	call 0xb9fda
	bra LOOP
DONE:
	;; $BA006
	retx %a0			; 4ed0
