remap_rom:
	;; $04AFE
	movel %a0@,%d3
	subl %a2,%d3
	addal %d3,%a0
	addal %d3,%a1
	ret							; bra $02E38
