b9fda:
	moveb #48,%a3@(2,%d3:l)
	moveb %d0,%a3@(6,%d3:l)
LOOP:	
	;; $B9FE4
	movew #1,%d0
	moveb %d0,%a3@(2,%d3:l)
	btst #0,%a3@(2)
	beq LOOP
	ret
