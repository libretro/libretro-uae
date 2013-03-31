	SECTION code
	xdef _mousehackint
_mousehackint:
	moveq.l #1,d0
	jsr.l $F0FF70
	move.l d0,d1
 	moveq.l #2,d0
	jsr.l $F0FF70
	cmp.l (a1),d1
	bne l1
	cmp.l 4(a1),d0
	beq l2
l1:
	move.l d0,4(a1)
	move.l d1,(a1)
	move.l 8(a1),d0
	move.l 12(a1),a1
	move.l 4.w,a6
	jsr -324(a6) ; Signal
l2:
	lea $DFF000,a0
	moveq.l #0,d0
	rts

	END
