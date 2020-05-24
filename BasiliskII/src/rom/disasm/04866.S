init_hw3:	
	;; $04866
	movel %a1@(28),%d3
	btst #22,%d3
	beq TEST_MCU
	lea HWID_REG,%a2		; =0x5FFFFFFC
	movel %a2@,%d3			
	andil 0xff00,%d3		
	moveal %a0@(52),%a2		
	clrb %a2@(4)
	moveb 0x40,%a2@(1)
	moveb #3,%a2@(5)
	cmpiw 0x2000,%d3
	bge TEST_MCU
	lea HWID_REG,%a2
	movel %a2@,%d3
	andiw 0x7FF,%d3
	cmpiw #3,%d3
	beq TEST_MCU
	cmpiw 0x101,%d3
	beq TEST_MCU
	moveal %a0@(52),%a2
	moveb #2,%a2@(5)
	;; $048BC
TEST_MCU:	
	btst #21,%d0
	beq TEST_JAWS
	moveal %a0@(84),%a3
	movel 0x124F0810,%d4
	moveq #31,%d3
	moveal %a0@(44),%a2
	bclr #5,%a2@(1024)
	btst #5,%a2@
	beq NUBUS_ACK1_R
	movel 0x138B0810,%d4
NUBUS_ACK1_R:
	;; $048E4
	lea %a3@,%a2
NUBUS_LOOP:
	;; $048E6
	movel %d4,%a2@+		
	lsrl #1,%d4			
	dbf %d3,NUBUS_LOOP
	moveq #0,%d4		
	movel %d4,%a2@+		
	movel %d3,%a3@(160)	
	movel %d3,%a3@(164)	
	movel %d3,%a3@(168)	
	movel %d3,%a3@(172)	
	movel %d3,%a3@(176)	
	movel %d3,%a3@(180)	
	movel %a3@(72),%d4	
	btst #0,%d4			
	beq MCU_48_TRUE
	moveq #0,%d4		
	bra MCU_48_DONE
MCU_48_TRUE:
	;; $04918
	moveq #1,%d4			
MCU_48_DONE:
	;; $0491A
	movel %d4,%a2@			
	movel %d3,%a3@(184)		
TEST_JAWS:	
	;; $04920
	btst #22,%d0			
	beq TEST_MEMCRJR
	moveal %a0@(88),%a2		
	moveal %a2,%a3			
	addal 0x34000,%a3		
	moveb %a3@,%d3			
	andib #1,%d3			
	moveb %d3,%a2@			
TEST_MEMCRJR:
	;; $0493A
	movel %a1@(28),%d3		
	btst #23,%d3			
	beq INIT_HW3_DONE		
	movel 0x5ffffffc,%d0	
	cmpiw 0x2BAD,%d0		
	beq QUDRA_CENTRIS 
	lea 0x50f0e000,%a2		
	movew %d0,%d3			
	swap %d3		
	movew %d0,%d3			
	andiw #3,%d3			
	movew %pc@(LC_DATA1,%d3:w:2),%d0
	rorl #6,%d0			
	movel %d0,%a2@(124)	
	roll #6,%d0			
	movel %d0,%a2@(48)	
	movew %pc@(0x4a28,%d3:w:2),%d0
	rorl #6,%d0			
	movel %d0,%a2@(124)	
	roll #6,%d0			
	movel %d0,%a2@(52)	
	lea 0x50f18000,%a2	
	moveb %pc@(0x4a30,%d3:w),%d0
	moveb %d0,%a2@		
	movew %pc@(0x4a34,%d3:w:2),%d0
	movew %d0,%a2@(1536)
	swap %d3			
	rorw #2,%d3			
	andiw #511,%d3		
	cmpiw #140,%d3		
	bne INIT_HW3_DONE	
	bset #4,%a2@		
	bset #3,%a2@(256)	
	bra INIT_HW3_DONE
LC_DATA1:	
	;; $049BC
	.short 0x0101
	.short 0x001A
	.short 0x00DC
	.short 0x02dc
QUDRA_CENTRIS:	
	;; $049C4
	moveal %a0@(8),%a2	
	moveq #40,%d3		
	moveb %d3,%a2@(1536)
	moveq #20,%d3		
	andb %a2@(7680),%d3	
	lsrb #2,%d3			
	btst #2,%d3			
	beq 0x49e0			
	bset #1,%d3			
049e0:
	andib #3,%d3
	lea 0x50f0e000,%a2
	movew %pc@(0x4a20,%d3:w:2),%d0
	movel %d0,%a2@(48)
	movew %pc@(0x4a28,%d3:w:2),%d0
	movel %d0,%a2@(52)
	lea 0x50f18000,%a2
	movel %a2@,%d0
	andib 0xF7,%d0
	orb %pc@(0x4a30,%d3:w),%d0
	movel %d0,%a2@
	movew %pc@(0x4a34,%d3:w:2),%d0
	movel %d0,%a2@(1536)
	bra INIT_HW3_DONE
04A20:
	.short 0x0101
	.short 0x001A
	.short 0x00A3
	.short 0x02f4
04a28:
	.short 0x011D
	.short 0x016B
	.short 0x01E7
	.short 0x0255
04A30:
	.byte 1
	.byte 1
	.byte 0
	.byte 0
04A34:
	.short 0x0280
	.short 0x01E0
	.short 0x00D5
	.short 0x0000
INIT_HW3_DONE:
	;; $04A3C
	movel %a1@(28),%d3
	btst #27,%d3
	beq DONE
	lea 0x50f18000,%a2
	moveb #-112,%a2@(1280)
	moveb #-73,%a2@(1281)
DONE:
	;; $04A5C
	retx %a4
