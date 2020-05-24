0008c:
	movew 0x2700,%sr		; $2700      
	call init_mmu			; $04052
	reset					
	call init_hw			; $02E00
	call 0x8ac00		
	braw 0x8e0			
    
