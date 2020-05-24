.macro call operand 
	lea %pc@(6),%fp
	jmp operand
.endm
.macro callx operand, return
	lea \return,%fp
	jmp operand
.endm

.macro ret
	jmp %fp@
.endm

.macro retr reg
	jmp \reg
.endm
