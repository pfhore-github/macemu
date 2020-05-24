8ac00:
	btst #24,%d2			; 0802 0018
	beq DONE
	moveal %fp,%a5
	lea PB_NEXT,%fp
	moveal 0xd18,%a2
	cmpil 0x53415241,%a2@(274)	; "SARA"
	bne PB_NEXT
	clrl %a2@(274)
	moveal %a2@(18),%a0
	jmp %a0
	nop		
PB_NEXT:
	;; $8AC28
	moveal %a5,%fp
	moveal %fp,%sp
	call 0xB29C4
	moveq #0,%d2
	call getHWInfo
	movel %d2,%d4	  
	moveq #16,%d1	  
	moveal %a0@(8),%a2
	call 0x4B53C
	moveq #-88,%d0	  
	subb %d1,%d0	  
	moveq #0,%d1	  
	tstb %d0		  
	bne 0x8ac96		  
	moveq #115,%d1	  
	movel %a1@(60),%d0
	beq 0x8ac7c		
	moveal %a1,%a2	
	addal %d0,%a2	
	addal %a2@,%a2	
	moveb %a2@,%d1	
	addqb #3,%d1	
8ac7c:
	moveal %a1,%a0			
	addal %a0@,%a0			
	moveal %a0@(8),%a2		
	call 0x4B53C
	andib #2,%d1			
8ac96:
	movel %d4,%d2			; 2404
8ac98:	 moveal %a1,%a0			; 2049
8ac9a:	 addal %a0@,%a0			; d1d0
8ac9c:	 cmpib #12,%d2			; 0c02 000c
8aca0:	 beq 0x8acba			; 6718
8aca2:	 cmpib #9,%d2			; 0c02 0009
8aca6:	 bne 0x8acec			; 6644
8aca8:	 moveal %a0@(88),%a1		; 2268 0058
8acac:	 movel #212992,%d0		; 203c 0003 4000
8acb2:	 btst #0,%a1@(0000000000000000,%d0:l); 0831 0000 0800
8acb8:	 beq 0x8ace8			; 672e
8acba:
	moveal %a0@(88),%a1		; 2268 0058
8acbe:	 moveq #2,%d2			; 7402
8acc0:	 andb %a1@(16384),%d2		; c429 4000
8acc4:	 cmpb %d1,%d2			; b401
8acc6:	 beq 0x8ace8			; 6720
8acc8:	 moveb %d1,%a1@(16384)		; 1341 4000
8accc:	 movew #208,%d3			; 363c 00d0
8acd0:	 lea 0x8acde,%fp		; 4dfa 000c
8acd4:	 lea 0xfffbcb6c,%a2		; 45f9 fffb cb6c
8acda:	 jmp %pc@(0x8acd4,%a2:l)	; 4efb a8f8[ADDR=$100047840]
8acde:	 movel #262144,%d3		; 263c 0004 0000
8ace4:	 subql #1,%d3			; 5383
8ace6:	 bne 0x8ace4			; 66fc
8ace8:	 bra 0x8adc0			; 6000 00d6
8acec:
	cmpib #10,%d2			; 0c02 000a
8acf0:	 bne 0x8adc0			; 6600 00ce
8acf4:	 orib #1,%d1			; 0001 0001
8acf8:	 btst #1,%d1			; 0801 0001
8acfc:	 bne 0x8ad14			; 6616
8acfe:	 moveq #7,%d2			; 7407
8ad00:	 andw %a1@(68),%d2		; c469 0044
8ad04:	 cmpiw #5,%d2			; 0c42 0005
8ad08:	 beq 0x8ad10			; 6706
8ad0a:	 cmpiw #2,%d2			; 0c42 0002
8ad0e:	 bne 0x8ad14			; 6604
8ad10:	 bclr #0,%d1			; 0881 0000
8ad14:	 moveal %a0@(52),%a1		; 2268 0034
8ad18:	 moveq #-8,%d2			; 74f8
8ad1a:	 andb %a1@(16),%d2		; c429 0010
8ad1e:	 orb %d2,%d1			; 8202
8ad20:	 moveb %d1,%a1@(16)		; 1341 0010
8ad24:	 movel #369,%d3			; 263c 0000 0171
8ad2a:	 lea 0x8ad38,%fp		; 4dfa 000c
8ad2e:	 lea 0xfffbcb12,%a2		; 45f9 fffb cb12
8ad34:	 jmp %pc@(0x8ad2e,%a2:l)	; 4efb a8f8[ADDR=$100047840]
8ad38:	 moveal %sp,%a5			; 2a4f
8ad3a:	 bset #27,%d7			; 08c7 001b
8ad3e:	 call 0x8ad6c			; 4dfa 0006 4efa 0028
8ad46:	 bclr #27,%d7			; 0887 001b
8ad4a:	 bra 0x8adc0			; 6074
8ad4c:	 bset %d4,%d0			; 09c0
8ad4e:	 bchg %d4,%d0			; 0940
8ad50:	 .short 0x044c			; 044c
8ad52:	 andib #-64,%d0			; 0200 03c0
8ad56:	 btst %d7,%d0			; 0f00
8ad58:	 orib #16,%a0@			; 0010 0010
8ad5c:	 btst %d0,%d0			; 0100
8ad5e:	 bclr %d4,%d0			; 0980
8ad60:	 .short 0x044c			; 044c
8ad62:	 bset %d1,%d0			; 03c0
8ad64:	 btst %d7,%d0			; 0f00
8ad66:	 orib #16,%a0@			; 0010 0010
8ad6a:	 btst %d0,%d0			; 0100
8ad6c:	 cmpil #1383428979,0xfeffffe4	; 0cb9 5275 7373
8ad72:	 				; feff ffe4
8ad76:	 bne 0x8adbe			; 6646
8ad78:	 cmpil #1398227745,0xfeffffe8	; 0cb9 5357 4321
8ad7e:	 				; feff ffe8
8ad82:	 bne 0x8adbe			; 663a
8ad84:	 bset #1,0xfee00021		; 08f9 0001 fee0
8ad8a:	 				; 0021
8ad8c:	 lea 0xfee08000,%a0		; 41f9 fee0 8000
8ad92:	 lea 0x8ad4c,%a2		; 45fa ffb8
8ad96:	 moveq #18,%d1			; 7212
8ad98:	 lea 0x8ad9e,%a1		; 43fa 0004
8ad9c:	 bra 0x8ada6			; 6008
8ad9e:	 addqw #2,%a0			; 5448
8ada0:	 moveq #14,%d1			; 720e
8ada2:	 lea 0x8adb6,%a1		; 43fa 0012
8ada6:	 moveb %a0@,%d2			; 1410
8ada8:	 bra 0x8adb0			; 6006
8adaa:	 movel %sp@,%sp@		; 2e97
8adac:	 movel %sp@,%sp@		; 2e97
8adae:	 moveb %a2@+,%a0@		; 109a
8adb0:	 dbf %d1,0x8adaa		; 51c9 fff8
8adb4:	 retx %a1			; 4ed1
8adb6:	 bclr #1,0xfee00021		; 08b9 0001 fee0
8adbc:	 				; 0021
8adbe:	 ret				; 4ed6
8adc0:
	moveal %sp,%fp			; 2c4f
8adc2:	 moveal %fp,%sp			; 2e4e
8adc4:	 moveq #0,%d2			; 7400
8adc6:	 lea 0x8add4,%fp		; 4dfa 000c
8adca:	 lea 0xfff7819a,%a0		; 41f9 fff7 819a
8add0:	 jmp %pc@(0x8adca,%a0:l)	; 4efb 88f8[ADDR=$100002f64]
8add4:	 cmpib #12,%d2			; 0c02 000c
8add8:	 beq 0x8adee			; 6714
8adda:	 cmpib #10,%d2			; 0c02 000a
8adde:	 bne 0x8aeaa			; 6600 00ca
8ade2:	 moveal %a0@(52),%a2		; 2468 0034
8ade6:	 bset #3,%a2@(33)		; 08ea 0003 0021
8adec:	 bra 0x8ae14			; 6026
8adee:	 movel #-369097760,%d3		; 263c ea00 03e0
8adf4:	 lea 0x8ae02,%fp		; 4dfa 000c
8adf8:	 lea 0xfffbca48,%a2		; 45f9 fffb ca48
8adfe:	 jmp %pc@(0x8adf8,%a2:l)	; 4efb a8f8[ADDR=$100047840]
8ae02:	 moveb #10,%d3			; 163c 000a
8ae06:	 lea 0x8ae14,%a5		; 4bfa 000c
8ae0a:	 lea 0xfffbcab8,%a4		; 49f9 fffb cab8
8ae10:	 jmp %pc@(0x8ae0a,%a4:l)	; 4efb c8f8[ADDR=$1000478c2]
8ae14:	 moveal %a0@(56),%a0		; 2068 0038
8ae18:	 moveal %sp,%a5			; 2a4f
8ae1a:	 bset #27,%d7			; 08c7 001b
8ae1e:	 call 0x8ae4a			; 4dfa 0006 4efa 0026
8ae26:	 bne 0x8aeaa			; 6600 0082
8ae2a:	 moveq #7,%d0			; 7007
8ae2c:	 andb %a0@(1),%d0		; c028 0001
8ae30:	 muluw #11,%d0			; c0fc 000b
8ae34:	 lea 0x8ae52,%a2		; 45fa 001c
8ae38:	 addal %d0,%a2			; d5c0
8ae3a:	 addqw #3,%a0			; 5648
8ae3c:	 movel %a2@+,%a0@+		; 20da
8ae3e:	 movel %a2@+,%a0@+		; 20da
8ae40:	 lea %a0@(18),%a0		; 41e8 0012
8ae44:	 moveb %a2@+,%a0@+		; 10da
8ae46:	 movew %a2@+,%a0@+		; 30da
8ae48:	 bra 0x8aeaa			; 6060
8ae4a:	 moveb %a0@(1),%d0		; 1028 0001
8ae4e:	 moveq #0,%d0			; 7000
8ae50:	 ret				; 4ed6
8ae52:	 moveb %d0,%d0			; 1000
8ae54:	 bcc 0x82e58			; 6400 8002
8ae58:	 oril #786,%a0@-		; 00a0 0000 0312
8ae5e:	 oriw #128,%a4@-		; 0064 0080
8ae62:	 andib #0,%d0			; 0200 ff00
8ae66:	 orib #0,%d3			; 0003 1000
8ae6a:	 bcc 0x82e6e			; 6400 8002
8ae6e:	 .short 0x00ff			; 00ff
8ae70:	 orib #16,%d0			; 0000 0310
8ae74:	 oriw #128,%a4@-		; 0064 0080
8ae78:	 andib #0,%d0			; 0200 a000
8ae7c:	 orib #0,%d3			; 0003 1000
8ae80:	 bcc 0x82e87			; 6400 8005
8ae84:	 oril #784,%a0@-		; 00a0 0000 0310
8ae8a:	 oriw #128,%a4@-		; 0064 0080
8ae8e:	 btst %d2,%d0			; 0500
8ae90:	 .short 0xa000			; a000
8ae92:	 orib #0,%d3			; 0003 1000
8ae96:	 bls 0x82e9d			; 6300 8005
8ae9a:	 oril #784,%a4@+		; 009c 0000 0310
8aea0:	 oriw #128,%a4@-		; 0064 0080
8aea4:	 btst %d2,%d0			; 0500
8aea6:	 .short 0xa000			; a000
8aea8:	 orib #-121,%d3			; 0003 0887
8aeac:	 orib #79,%a3@+			; 001b 2c4f
DONE:	
	ret
