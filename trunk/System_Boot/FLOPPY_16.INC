
;*******************************************************
;
;	Floppy16.inc
;		Floppy drive interface routines
;
;	OS Development Series
;*******************************************************

%ifndef __FLOPPY_16_INC__INCLUDED__
%define __FLOPPY_16_INC__INCLUDED__

bits	16

datasector  dw 0x0000
cluster     dw 0x0000

aSector db 0x00
aHead   db 0x00
aTrack  db 0x00

;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************;

ClusterLBA:
          sub     ax, 0x0002                          ; zero base cluster number
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; convert byte to word
          mul     cx
          add     ax, WORD [datasector]               ; base data sector
          ret

;************************************************;
; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

LBACHS:
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbSectorsPerTrack]           ; calculate
          inc     dl                                  ; adjust for sector 0
          mov     BYTE [aSector], dl
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbHeadsPerCylinder]          ; calculate
          mov     BYTE [aHead], dl
          mov     BYTE [aTrack], al
          ret


;************************************************;
; Reads a series of sectors
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer to read to
;************************************************;

ReadSectors:
	.MAIN:
		mov     di, 0x0005                          ; five retries for error
	.SECTORLOOP:
		push    ax
		push    bx
		push    cx
		call    LBACHS                              ; convert starting sector to CHS
		mov     ah, 0x02                            ; BIOS read sector
		mov     al, 0x01                            ; read one sector
		mov     ch, BYTE [aTrack]       		      ; track
		mov     cl, BYTE [aSector]           		  ; sector
		mov     dh, BYTE [aHead]             		  ; head
		mov     dl, BYTE [bsDriveNumber]            ; drive
		int     0x13                                ; invoke BIOS
		jnc     .SUCCESS                            ; test for read error
		xor     ax, ax                              ; BIOS reset disk
		int     0x13                                ; invoke BIOS
		dec     di                                  ; decrement error counter
		pop     cx
		pop     bx
		pop     ax
		jnz     .SECTORLOOP                         ; attempt to read again
		int     0x18
	.SUCCESS:
		pop     cx
		pop     bx
		pop     ax
		add     bx, WORD [bpbBytesPerSector]        ; queue next buffer
		inc     ax                                  ; queue next sector
		loop    .MAIN                               ; read next sector
		ret

%endif		;__FLOPPY_16_INC__INCLUDED__
