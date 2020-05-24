	;; $B29C4
init_pb_dio:	
	movel %fp,%d5
	moveq #0,%d2
	call getHWInfo
	lea 0xb2a74,%a1
DATA1_LOOP:
	;; $B29DA
	addqw #8,%a1
	movew %a1@+,%d1
	beq DONE
	cmpb %d2,%d1
	bne DATA1_LOOP
	movel %a1@+,%d1
	addal %a1@,%a1
	movew 0xE2,%d3
	call send_to_pb_dword
	bne FAIL
	call recv_from_pb
	cmpib #98,%d3
	beq DONE
	movew 0xE1,%d3
	call send_to_pb_dword
	bne FAIL
LOOP:
	;; $B2A26
	moveb %a1@+,%d3
	call send_to_pb_byte
	dbne %d1,LOOP
	bne LOOP
	moveq #30,%d2
FINAL_LOOP:
	;; $B2A3E
	movew 0xE2,%d3
	call send_to_pb_dword
	dbeq %d2,FINAL_LOOP
	bne FAIL
	call recv_from_pb
	cmpib 0x62,%d3
	beq DONE
FAIL:	
	;; $B2A6A
	moveq #0,%d6
	moveq #16,%d7
	lea 0x2600,%sp
	bra 0x4aa98
DONE:	
	;; $B2A78
	moveal %d5,%fp
	ret
