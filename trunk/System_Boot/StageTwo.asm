[bits 16]
[org 0xBE00]

jmp short startup
nop

;----------------------------------------------------
; BIOS Parameter Block
;----------------------------------------------------
bpbOEM					db "MyOS 0.1"
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 		DB 2
bpbRootEntries: 		DW 224
bpbTotalSectors: 		DW 2880
bpbMedia: 				DB 0xf0  ;; 0xF1
bpbSectorsPerFAT: 		DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 		DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused:               DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MyOS v. 0.1"
bsFileSystem: 	        DB "FAT12   "

startup:
    ;----------------------------------------------------
    ; Pointer Setup (STAGEONE loads us to 0xBE00)
    ;---------------------------------------------------- 
	cli
    xor     ax, ax
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
	
    ;----------------------------------------------------
    ; create stack
    ;---------------------------------------------------- 
    mov     ax, 0x9000
    mov     ss, ax
    mov     sp, 0xFFFF
	sti
    
    mov  [bootdevice], dl
    
    ;----------------------------------------------------
    ; My Codes Here
    ;----------------------------------------------------
	mov		si, BOOTMSG
	call	puts
	;call	initFAT12							; should be initialized already
	mov		ax, 0x07C0							; all data offset is from 0x7C00
	mov		es, ax								; Select Segment 0x07C0
	
	mov     bx, 0x5000                          ; destination for image
	push    bx
	mov		si, ImageName
	call	FindFile
	cmp		ax, 1
	je		TimeToGO
	
    ;----------------------------------------------------
    ; End - Error Has Occurred...
    ;----------------------------------------------------
ErrorSub:
	mov		si, ERRMSG
	call	puts
    cli
    hlt
	
TimeToGO:
	mov		ax, 0x0BE0							; Point ES back to 0x0BE0
	mov		es, ax	
	
	mov		si, DONEMSG
	call	puts	
	
	;----------------------------------------------------
	; Should enable A20, and Enter PMode - Copy Stage3 and run
	;----------------------------------------------------
	mov		si, GDTMSG
	call	puts
	call	initGDT					; Install GDT
	mov		si, DONEMSG
	call	puts
	mov		si, A20MSG
	call	puts
	call	EnableA20				; Enable A20
	mov		si, DONEMSG
	call	puts
	mov		si, PModeMSG
	call	puts
	jmp		goPMode					; Enable PMode
	
	jmp		ErrorSub
	
goPMode:
	cli
	;lgdt 	[toc]
	mov		eax, cr0
	or		al, 1
	mov		cr0, eax
	jmp		0x8:inPMode				; Jump To 32-bit code
	
;----------------------------------------------------
; Drivers
;----------------------------------------------------
%include "floppy_16.inc"
%include "fat12.inc"
%include "stdio_16.inc"
%include "stdio_32.inc"
%include "A20.inc"
%include "GDT.inc"

;----------------------------------------------------
; Variables
;----------------------------------------------------
ImageName	db "STAGE3  BIN"
BOOTMSG		db 0xA, 0xD, "Loading Stage Three... ", 0
A20MSG		db "Enableing A20...", 0
PModeMSG	db "Enableing Protected Mode...", 0
GDTMSG		db "Setting up GDT...", 0
CopyMSG		db "Coppying Stage Three...", 0
LaunchMSG	db "Launching Stage Three...", 0
ERRMSG		db "ERROR!",0xA, 0xD, 0
DONEMSG		db "Done!", 0xA, 0xD, 0
bootdevice	db 0
ImageSize	db 0

;----------------------------------------------------
; Protected Mode
;----------------------------------------------------
[bits 32]

inPMode:
	mov		ax, 0x10					; set data segments to data selector (0x10)
	mov		ds, ax
	mov		ss, ax
	mov		es, ax
	mov		esp, 0x90000					; stack begins from 90000h
	sti

	mov		ebx, DONEMSG
	call	print
	
;CopyImage:
	;mov		ebx, CopyMSG
	;call	print
	
	;mov	eax, dword [ImageSize]
	;movzx	ebx, word [bpbBytesPerSector]
	;mul	ebx
	;mov	ebx, 4
	;div	ebx
	;cld
	;mov    esi, 0xCC00
	;mov	edi, 0x300000
	;mov	ecx, eax
	;rep	movsd
	
	;mov		ebx, DONEMSG
	;call	print
	;mov		ebx, LaunchMSG
	;call	print
	
	;jmp	0x8:0x300000
	cli
	hlt