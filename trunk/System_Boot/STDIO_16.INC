
;*************************************************
;	stdio.inc
;		-Input/Output routines
;
;	OS Development Series
;*************************************************

%ifndef __STDIO_16_INC__INCLUDED__
%define __STDIO_16_INC__INCLUDED__


;==========================================================
;
;	 16 Bit Real Mode Routines
;==========================================================


;************************************************;
;	Puts16 ()
;		-Prints a null terminated string
;	DS=>SI: 0 terminated string
;************************************************;

bits	16

puts:
		pusha				; save registers
.Loop1:
		lodsb				; load next byte from string from SI to AL
		or	al, al			; Does AL=0?
		jz	PutsDone		; Yep, null terminator found-bail out
		mov	ah, 0eh			; Nope-Print the character
		int	10h			; invoke BIOS
		jmp	.Loop1			; Repeat until null terminator found
PutsDone:
		popa				; restore registers
		ret				; we are done, so return
		
%endif	;__STDIO_16_INC__INCLUDED__
