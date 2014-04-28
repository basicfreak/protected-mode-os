
;*******************************************************
;	This One Is Mine ;)
;*******************************************************

%ifndef __FAT12_INC__INCLUDED__
%define __FAT12_INC__INCLUDED__

bits	16



; Only have a limited amount of space
; Load all FATs and Root Dir at once just after boot sector
; Assume 32 sectors offset 1
; No input needed and will store at
; 0x7E00 - 0xBDFF
; 0x07C0:0x0200 - 0x07C0:0x41FF
; Remember this as we can cheat in Kernel and Stage Two if we know we have this boot sector.
; Plan to include this file on stage two (shouldn't need to call init then)
; Not even sure if we can read 32 sectors at once...
initFAT12:
	mov cx, 32
	mov ax, 1
	mov bx, 0x200
	call ReadSectors
	ret

; input word[cluster] = current cluster
; output word[cluster] = next cluster	
nextCluster:
	;push	ax
	;push	cx							; store registers
	;push	dx
	push	bx
	mov     ax, WORD [cluster]	; identify current cluster
	mov     cx, ax			; copy current cluster
	mov     dx, ax
	shr     dx, 0x0001		; divide by two
	add     cx, dx			; sum for (3/2)
	mov		bx, 0x200			; FAT offset
	add		bx, cx
	mov		dx, WORD [es:bx]
	test	ax, 0x0001		; test for odd or even cluster
	jnz		.ODD_CLUSTER
	.EVEN_CLUSTER:
		and		dx, 0000111111111111b	; take low 12 bits
		jmp		.DONE
	.ODD_CLUSTER:
		shr		dx, 0x0004		; take high 12 bits
	.DONE:
		mov		WORD [cluster], dx
	pop	bx						; restore registers and return
	;pop	dx
	;pop	cx
	;pop ax
	ret
	
; Due to limited space lets make this an all in one finder and loader.
; input SI => FileName (11)
; input ES:BX => Buffer
;
; return AX = -1 on error
FindFile:
	push	cx							; store registers
	push	dx
	push	bx
	mov		bx, si						; copy filename for later
	mov     cx, WORD [bpbRootEntries]	; loop register
	mov     di, 0x2600					; root dir mem location
	cld
	.LOOP:
		push    cx
		mov     cx, 11					; eleven character name. Image name is in SI
		mov		si, bx						; image name is in BX
		push    di
		rep		cmpsb							; test for entry match
		pop     di
		je      .FOUND
		pop     cx
		add     di, 32					; queue next directory entry
		loop    .LOOP
	.NOTFOUND:
		pop	bx						; restore registers and return
		pop	dx
		pop	cx
		mov	ax, -1						; set error code
		ret
	.FOUND:
		mov		dx, WORD [es:di + 0x001A];  ; start cluster		
		mov		WORD [cluster], dx	
		pop ax
		pop	bx						; restore registers
		pop	dx
		pop	cx
; Found file lets load it. (hopefully root offset is in ax.)
	;************************************************;
	; Reads a series of sectors
	; CX=>Number of sectors to read
	; AX=>Starting sector
	; ES:BX=>Buffer to read to
	;************************************************;
	push	cx							; store registers
	push	dx
	
	.ReadLoopMe:
		mov		si, dotMSG
		call	puts
		
		mov 	ax, WORD [cluster]			; cluster number
		add		ax, 31						; sector offset
		mov		cx, 1						; read one sector
		push bx
		call	ReadSectors					; read
		call	nextCluster					; get next cluster		
		pop bx
		add		bx, 0x200					; add 512 bytes to buffer location
		mov		ax, WORD [cluster]
		cmp		ax, 0xff8
		jl		.ReadLoopMe
	pop	dx
	pop	cx
	mov	ax, 1
	ret

	dotMSG db ".", 0
%endif		;__FAT12_INC__INCLUDED__





