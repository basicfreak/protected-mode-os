[bits 16]
[org 0]

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
    ; Pointer Setup (BIOS loads us to 0x7C00)
    ;---------------------------------------------------- 
    cli
    mov     ax, 0x07C0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
	
    ;----------------------------------------------------
    ; create stack
    ;---------------------------------------------------- 
    mov     ax, 0x500
    mov     ss, ax
    mov     sp, 0x2000
	sti
    
    mov  [bootdevice], dl
    
    ;----------------------------------------------------
    ; My Codes Here
    ;----------------------------------------------------
	mov		si, BOOTMSG
	call	puts
	
	call	initFAT12
	mov     bx, 0x4200                          ; destination for image
	mov		si, ImageName
	call	FindFile
	cmp		ax, 1
	je		jumpTime
    ;----------------------------------------------------
    ; End - Error Has Occurred...
    ;----------------------------------------------------
ErrorSub:
	mov		si, ERRMSG
	call	puts
    cli
    hlt
	
jumpTime:
	mov		si, DONEMSG
	call	puts
	mov		dl, [bootdevice]
	
	push	WORD 0x0BE0
	push	WORD 0x0000
	retf
	;mov bx, 0x4200
	;jmp [es:bx]
	jmp		ErrorSub
;----------------------------------------------------
; Drivers
;----------------------------------------------------
%include "floppy_16.inc"
%include "fat12.inc"
%include "stdio_16.inc"

;----------------------------------------------------
; Variables
;----------------------------------------------------
ImageName	db "STAGETWOBIN"
BOOTMSG		db "Loading Stage Two...", 0
ERRMSG		db "ERROR!", 0
DONEMSG		db "Done!", 0
bootdevice	db 0
ImageSize	db 0

;----------------------------------------------------
; Boot Signature
;----------------------------------------------------
times 510-($-$$) db 0
dw 0xAA55

;----------------------------------------------------
; FAT TABLE 1
;----------------------------------------------------
db 0xF0
db 0xFF
db 0xFF
times 0x1600-($-$$) db 0

;----------------------------------------------------
; FAT TABLE 2
;----------------------------------------------------
db 0xF0
db 0xFF
db 0xFF
times 0x2800-($-$$) db 0

;----------------------------------------------------
; Pad to 1.44MB this is a floppy image not just bootsector
;----------------------------------------------------
times 0x168000-($-$$) db 0