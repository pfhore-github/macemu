b9f78:	 moveal %fp,%a5			; 2a4e
b9f7a:	 btst #17,%d0			; 0800 0011
b9f7e:	 beq 0xb9f8e			; 670e
b9f80:	 lea 0xb9f8e,%fp		; 4dfa 000c
b9f84:	 lea 0xfff8df72,%a0		; 41f9 fff8 df72
b9f8a:	 jmp %pc@(0xb9f84,%a0:l)	; 4efb 88f8[ADDR=$100047ef6]
b9f8e:	 tstb %a3@(0000000000000000,%d3:l); 4a33 3800
b9f92:	 lea 0xb9fc0,%a1		; 43fa 002c
b9f96:	 moveb %a1@+,%d0		; 1019
b9f98:	 bmi 0xb9fa4			; 6b0a
b9f9a:	 moveb %d0,%a3@(0000000000000002,%d3:l); 1780 3802
b9f9e:	 moveb %a1@+,%a3@(0000000000000002,%d3:l); 1799 3802
b9fa2:	 bra 0xb9f96			; 60f2
b9fa4:	 bset #17,%d7			; 08c7 0011
b9fa8:	 moveb %a3@(6),%d0		; 102b 0006
b9fac:	 bset #3,%a2@(1536)		; 08ea 0003 0600
b9fb2:	 moveb %a2@(7680),%d0		; 102a 1e00
b9fb6:	 bclr #3,%d0			; 0880 0003
b9fba:	 moveb %d0,%a2@(7680)		; 1540 1e00
b9fbe:	 retx %a5			; 4ed5
