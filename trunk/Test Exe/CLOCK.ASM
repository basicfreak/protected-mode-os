[bits 32]
[org 0x800000]

[global entry]
[extern _main]

entry:
	call _main
	jmp entry
	
[global _GetTime]
	
_GetTime:
	push ax
		mov al, 0x00
		out 0x70, al
		in al, 0x71
		mov [_secs], al
		mov al, 0x02
		out 0x70, al
		in al, 0x71
		mov [_mins], al
		mov al, 0x04
		out 0x70, al
		in al, 0x71
		mov [_hours], al
	pop ax
	ret

[global _secs]
[global _mins]
[global _hours]
	
_secs db 0	
_mins db 0
_hours db 0
