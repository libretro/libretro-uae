
    ; uae-configuration Amiga-side part
    ; (c) Toni Wilen 2004

OUTBUFSIZE = 1000

	move.l a0,a3
	move.l d0,d3
	sub.l a4,a4
	moveq #20,d7

	lea $f0ff60,a5
	tst.l (a5)
	beq.s end

	move.l 4,a6
	lea dos(pc),a1
	moveq #0,d0
	jsr -$228(a6) ;OpenLibrary
	tst.l d0
	beq.s end
	move.l d0,a4
	move.l a4,a6

	moveq #-1,d6

loop
	lea outbuf,a2
	move.l #OUTBUFSIZE,-(sp) ;out len
	move.l a2,-(sp) ;out
	clr.b (a2)
	move.l d3,-(sp) ;param len
	move.l a3,-(sp) ;param
	move.l d6,-(sp) ;index
	moveq #82,d0
	move.l d0,-(sp)
	jsr (a5)
	lea 6*4(sp),sp
	move.l d0,d7

 	; do we have output?
	move.l d3,-(sp)
	tst.b (a2)
	beq.s noout
	jsr -$003c(a6) ;Output
	move.l d0,d4
	beq.s noout
	move.l d4,d1
	move.l a2,d2
	moveq #-1,d3
out1
	addq.l #1,d3
	tst.b (a2)+
	bne.s out1
	jsr -$0030(a6) ; Write
	move.l d4,d1
	lea lf(pc),a0
	move.l a0,d2
	moveq #1,d3
	jsr -$0030(a6) ; Write
noout
	move.l (sp)+,d3


	tst.l d7
	bpl.s end
	; list all -mode
	addq.l #1,d6
	bra loop
end
	move.l a4,d0
	beq.s end2
	move.l d0,a1
	move.l 4,a6
	jsr -$19e(a6) ;CloseLibrary
end2
	move.l d7,d0
	rts

dos	dc.b "dos.library",0
lf	dc.b 10

	section 2,bss

outbuf
	ds.b OUTBUFSIZE
