%ifndef __STDIO_32_INC__INCLUDED__
%define __STDIO_32_INC__INCLUDED__

[bits 32]
%define		VIDMEM	0xB8000
%define		COLS	80
%define		LINES	25
%define		CHAR_ATTRIB 7

_CurX db 0
_CurY db 0

;************************************************;
;	EBX = address of string to print
;************************************************;
print:
	pusha				; save registers
	push	ebx			; copy the string address
	pop	edi
.loop:
	mov	bl, byte [edi]		; get next character
	cmp	bl, 0			; is it 0 (Null terminator)?
	je	.done			; yep-bail out
	call	putch			; Nope-print it out
	inc	edi			; go to next character
	jmp	.loop

.done:
	mov	bh, byte [_CurY]	; get current position
	mov	bl, byte [_CurX]
	popa				; restore registers, and return
	ret
		
putch:
	pusha				; save registers
	mov		edi, VIDMEM		; get pointer to video memory
	xor		eax, eax			; clear eax
	mov		ecx, COLS*2			; Mode 7 has 2 bytes per char, so its COLS*2 bytes per line
	mov		al, byte [_CurY]	; get y pos
	mul		ecx					; multiply y*COLS
	push	eax					; save eax--the multiplication
	mov		al, byte [_CurX]	; multiply _CurX by 2 because it is 2 bytes per char
	mov		cl, 2
	mul		cl
	pop		ecx					; pop y*COLS result
	add		eax, ecx
	xor		ecx, ecx
	add		edi, eax			; add it to the base address
	cmp		bl, 0x0A		; \n
	je		.Row
	cmp		bl, 0x0D		; \r
	je		.Cret
	mov	dl, bl			; Get character
	mov	dh, CHAR_ATTRIB		; the character attribute
	mov	word [edi], dx		; write to video display
	inc	byte [_CurX]		; go to next character
	cmp	byte [_CurX], COLS		; are we at the end of the line?
	jge	.Row			; yep-go to next row
	jmp	.done			; nope, bail out
.Row:
	mov	byte [_CurX], 0		; go back to col 0
	inc	byte [_CurY]		; go to next row
	jmp .done
.Cret:
	mov byte [_CurX], 0
	jmp .done
.done:
	popa				; restore registers and return
	ret
	
%endif	;__STDIO_32_INC__INCLUDED__
