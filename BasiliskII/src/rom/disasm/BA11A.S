	andiw #127,%d0
	subib #48,%d0
	bmi RET_0
	cmpib #22,%d0
	bgt RET_0
	cmpib #9,%d0
	ble END
	subqb #7,%d0
END:
	;; $BA132
	ret
RET_0:
	;; $BA134
	clrw %d0
	ret
