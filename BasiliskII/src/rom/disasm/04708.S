	;; $04708
test_scsi_cmd:	
	tstb %a2@(192)
	cmpib #4,%a2@(192)
	ret
