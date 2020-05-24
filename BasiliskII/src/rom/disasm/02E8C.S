reset_via:	
	;; $02E8C
	btst #2,%d0
	beq init_via2
	moveal %a0@(8),%a2
	moveal %a1,%a3
	addal %a3@(40),%a3
	moveb %a3@+,%a2@(0x1E00)
	moveb %a3@+,%a2@(0x0600)
	moveb %a3@+,%a2@
	moveb %a3@+,%a2@(0x0400)
	moveb %a3@+,%a2@(0x1800)
	moveb %a3@+,%a2@(0x1600)
	moveb #127,%a2@(0x1C00)
init_via2:
	;;  $02EB8
	btst #11,%d0
	beq init_rbv
	moveal %a0@(44),%a2
	moveal %a1,%a3
	addal %a3@(44),%a3
	moveb %a2@(0x1E00),%d3
	orb %a3@+,%d3
	moveb %d3,%a2@(0x1E00)
	moveb %a3@+,%a2@(0x0600)
	moveb %a3@+,%a2@
	moveb %a3@+,%a2@(0x0400)
	moveb %a3@+,%a2@(0x1800)
	moveb %a3@+,%a2@(0x1600)
	moveb #127,%a2@(0x1C00)
	moveb #110,%a2@(0x0800)
	moveb #25,%a2@(0x0A00)
init_rbv:
	;; $02EF6
	btst #13,%d0
	beq done
	moveal %a0@(52),%a2
	moveb #127,%a2@(19)
	moveb #-113,%a2@		; 14bc 008f
	btst #20,%d1			; 0801 0014
	beq turn_off_video
	movec %vbr,%d3
	subil #36,%d3
	movec %d3,%vbr
	moveq #1,%d3
	moveal %sp,%a5
	moveal %fp,%a3
	bset #27,%d7
	lea fpu_test_done,%fp
	fnop
	moveq #0,%d3
fpu_test_done:
	;; $02F32
	tstb %d3
	beq 0x2f3a
	bclr #0,%a2@
02f3a:
	movec %vbr,%d3
	addil #36,%d3
	movec %d3,%vbr
	bclr #27,%d7
	moveal %a3,%fp
	moveb #127,%a2@(18)
	ret
turn_off_video:
	;; $02F56
	moveb #64,%a2@(16)		; 157c 0040 0010
	moveb #-1,%a2@(18)		; 157c 00ff 0012
done:
	;; $02F62
	ret
