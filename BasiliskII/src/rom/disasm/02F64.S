get_hw_info:
	;; $02F64
	bra test_model_id			; $04B6C
	
04B6C_FAIL:
	;;  $02F6A
	tstb %d2
	beq MB_CHECK
	
	lea OLD_MACHINE_TABLE,%a0	; $03260(-4)
	;; subqw #4, %a0
OLD_MACHINE_LOOP:
	;; $02F72
	addqw #4,%a0
	movel %a0@,%d0
    beq MB_CHECK
	lea %a0@(0,%d0:l),%a1
	cmpw %a1@(18),%d2		; gestaltID
	bne OLD_MACHINE_LOOP
	
04B6C_SUCCESS:
	;;  $02F82
	moveal %a1@,%a0		; motherboard_offset
	addal %a1,%a0		 
	movel %a1@(24),%d0			; HW_INIT_FLG
	beq update_init_flag
	jmp DONE

MB_CHECK:	
	;; $02F9E
	movel %fp,%d0				; STORE FP
	lea HWCHECK_MB_TABLES, %a1			; $03240
CHECK_MB_ADDR_NG:
	;; $02FA4
	moveal %a1,%a0
	addal %a1@+,%a0
	moveal %a0@(-8),%a2		; check_addr
	jmp %a0@(0,%a2:l)		; call check_handler
CHECK_MB_ADDR_OK:
	;; $02FB0
	moveal %d0,%a2			; %D0=FP
	movel %a0@(-20),%d0		; hw_init_flg
	call read_id_from_via	; $047CC
	moveal %a2,%fp			; restore FP
	moveb %a0@(-4),%d2		; generation
	lea OLD_MACHINE_TABLE,%a0			; OLD_MACHINE_TABLES
MACHINE_LOOP:	
	;; $02FC8
	movel %a0@+,%d0		 
	beq MB_CHECK
	lea %a0@(#-4,%d0:l),%a1		; via_machine_check
	cmpb %a1@(19),%d2		; generation
	bne MACHINE_LOOP
	movel %d1,%d0
	andl %a1@(32),%d0		; via_machine_mask
	cmpl %a1@(36),%d0		; via_machine_id
	bne MACHINE_LOOP
	bra 04B6C_SUCCESS

update_init_flag:
	;; $02FE4
	movel %a0@(-20),%d0		; hw_init_flag
	moveal %fp,%a0			; STORE FP
	btst #13,%d0				; RBV
	beq VIA2_CHECK
	moveal %a1@,%a2
	moveal %a1@(0x34,%a2:l),%a2; RBV
	lea %a2@(0x13),%a2
	call has_rbv				; $046D8
	beq VIA2_CHECK
	bclr #13,%d0		; RBV
	bclr #14,%d0		; VDAC
VIA2_CHECK:
	;;  $0300C
	bclr #11,%d0					 ; VIA2
	beq IOP_SWIM_CHECK
	moveal %a1@,%a2
	moveal %a1@(0x2c,%a2:l),%a2; VIA2
	lea %a2@($1C00),%a2		; VIA.vIER
	call 0x46aa			; has_via
	bne IOP_SWIM_CHECK
	bset #11,%d0		; VIA2
IOP_SWIM_CHECK:
	;; $0302A
	bclr #16,%d0	 ; IOP_SWIM
	beq IOP_SCC_CHECK
	moveal %a1@,%a2	           
	moveal %a1@(0x40,%a2:l),%a2; IOP_SWIM
	call 0x477c			; test_iop
	bne IOP_SCC_CHECK
	bclr #5,%d0
	bset #16,%d0
IOP_SCC_CHECK:	
	;; $03048
	bclr #17,%d0				; IOP_SCC
	beq SCSI_DMA_CHECK
	moveal %a1@,%a2	
	moveal %a1@(0x44,%a2:l),%a2	;IOP_SCC
	call 0x477c			; test_iop
	bne SCSI_DMA_CHECK
	bset #17,%d0
	bclr #3,%d0
	bclr #4,%d0
SCSI_DMA_CHECK:	
	;; $0306A
	bclr #15,%d0
	beq SCSI_CHECK
	moveal %a1@,%a2
	moveal %a1@(0x3c,%a2:l),%a2 
	call 0x4700			; test_scsi_io
	bne SCSI_CHECK
	bset #15,%d0
	bclr #8,%d0
	bclr #9,%d0
	bclr #10,%d0
SCSI_CHECK:
	;; $03090
	call scsi_check				; $04A96
	addal %a0@,%a0
DONE:	
	;; $02F90
	movel %a1@(16),%d2			; HW_CFG
	call check_flag				; $04B0C 

	;; $0309C
	movel %a1@(28),%d1
	bne HAS_ROM_FLG
	movel %a0@(-16),%d1
HAS_ROM_FLG:	
	;; $030A6
	bclr #27,%d7
	ret

HWCHECK_MB_TABLES:
	;; $03240
	
OLD_MACHINE_TABLE:	
	;; $03260
