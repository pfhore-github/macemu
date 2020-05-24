bd494:	 moveb %fp@(2048),%d0		; 102e 0800
bd498:	 cmpib #-68,%d0			; 0c00 00bc
bd49c:	 beq 0xbd4a4			; 6706
bd49e:	 cmpib #-69,%d0			; 0c00 00bb
bd4a2:	 bne 0xbd4b6			; 6612
bd4a4:	 moveb %fp@(2052),%d0		; 102e 0804
bd4a8:	 andib #12,%d0			; 0200 000c
bd4ac:	 cmpib #12,%d0			; 0c00 000c
bd4b0:	 beq 0x45e1e			; 67ff fff8 896c
bd4b6:	 moveb %fp@(2052),%d0		; 102e 0804
bd4ba:	 andib #3,%d0			; 0200 0003
bd4be:	 bra 0x45e18			; 60ff fff8 8958
