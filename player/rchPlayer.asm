	MODULE rch

chunks	equ 0		; chunks table adr

; rch.init
; in: HL = rch.adr

init	ld a,(hl)
	inc hl
	ld (rchAdr),hl
	ld (rchFrm),a
	xor a
	ld (rchCnt),a
	call outFrm
	ld (rchLoop),hl
	ret

; rch.play
; call to draw a next frame

play	ld a,(rchFrm)
	and a
	ret z
	ld hl,(rchAdr)
	call outFrm
	ld bc,(rchFrm)
	ld a,b
	inc a
	cp c
	jr c,ply1
	xor a
	ld hl,(rchLoop)
ply1	ld (rchCnt),a
	ld (rchAdr),hl
	ret

outFrm	ld de,0x4000
ofrm1	ld a,(hl)	; to skip
	inc hl
	add a,e
	ld e,a
	sbc a,a
	and 8
	add a,d
	ld d,a
	cp 0x58
	ret nc
	ld a,(hl)	; to draw
	inc hl
	and a
	jr z,ofrm1
ofrm2	exa
	ld c,(hl)	; chunk nr
	inc hl
	ld b,high chunks
	dup 8
	 ld a,(bc)
	 ld (de),a
	 inc b
	 inc d
	edup
	inc e
	jr z,ofrm3
	ld a,d
	sub 8
	ld d,a
	cp 0x58
	ret nc
ofrm3	exa
	dec a
	jr nz,ofrm2
	jr ofrm1

rchAdr	dw 0
rchLoop	dw 0
rchFrm	db 0
rchCnt	db 0

	ENDMODULE
