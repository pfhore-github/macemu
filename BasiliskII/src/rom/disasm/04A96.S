scsi_check:
	;; $04A96
	bclr #24,%d0				; SCSI_040
	beq IIFX_EXP1_CHECK
	moveal %a1@,%a2
	moveal %a1@(0x60,%a2:l),%a2
	call 0x4708					; SCSI CMD CHECK
	bne IIFX_EXP1_CHECK
	bset #24,%d0
	bclr #15,%d0
	bclr #8,%d0
	bclr #9,%d0
	bclr #10,%d00
	bclr #25,%d0
	beq IIFX_EXP1_CHECK
	moveal %a1@,%a2
	moveal %a1@(0x64,%a2:l),%a2
	call 0x4708
	bne IIFX_EXP1_CHECK
	bset #25,%d0
IIFX_EXP1_CHECK:	
	;; $04ADA
	bclr #20,%d0
	beq DONE
	moveal %a1@,%a2
	moveal %a1@(0x50,%a2:l),%a2
	call 0x4672
	bne DONE
	bset #20,%d0
DONE:
	;; $4AF4
	moveal %a0,%fp
	moveal %a1,%a0
	ret							;$03096
